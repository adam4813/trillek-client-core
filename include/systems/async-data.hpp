#ifndef ASYNCDATA_HPP_INCLUDED
#define ASYNCDATA_HPP_INCLUDED

#include <mutex>
#include <iterator>
#include "trillek-scheduler.hpp"

namespace trillek {

template<class T>
class HistoryMap {
public:
    HistoryMap(typename std::map<frame_tp,T>::const_iterator&& begin, typename std::map<frame_tp,T>::const_iterator&& end)
        : start(std::move(begin)), stop(std::move(end)), hsize(std::distance(stop, start)) {};

    typename std::map<frame_tp,T>::const_iterator& cbegin() {
        return start;
    }

    typename std::map<frame_tp,T>::const_iterator& cend() {
        return stop;
    }

    typename std::map<frame_tp,T>::size_type size() const {
        return hsize;
    }

private:
    typename std::map<frame_tp,T>::const_iterator start;
    typename std::map<frame_tp,T>::const_iterator stop;
    typename std::map<frame_tp,T>::size_type hsize;
};

template<class T,int HistorySize = 30>
class AsyncFrameData {
    typedef std::map<frame_tp,T> content_map;
public:
    AsyncFrameData() {
        for (auto i = 0; i < HistorySize; ++i) {
            datas.emplace_hint(datas.end(), std::make_pair<frame_tp,T>(frame_tp(frame_unit(i)), T()));
        }
    };

    /** \brief Return the frames between last_received and frame_requested
     *
     * last_received is not included, frame_requested is always included.
     *
     * last_received will be updated if some data is retrieved
     *
     * Actually return bound iterators on the map of data
     *
     * Blocks if the call is too early, and returns no data after 0.5s
     *
     * \param frame_requested const frame_tp& the "now" of the caller
     * \param last_received frame_tp& the last frame received by the caller
     * \return HistoryMap An object that with begin(), end() and size() functions
     *
     */
    HistoryMap<T> GetHistoryData(const frame_tp& frame_requested, frame_tp& last_received) const {
        std::unique_lock<std::mutex> locker(m_current);
        auto requested_index = datas.upper_bound(last_received);
        if (requested_index == datas.cend() || frame_requested > current_frame) {
            // we block until the frame time or after 500 ms
            if (! ahead_request.wait_for(locker, std::chrono::milliseconds(500), [&](){ return frame_requested <= current_frame; })) {
                // 500 ms have passed, let return a empty object
                return HistoryMap<T>(datas.cend(), datas.cend());
            }
            requested_index = datas.upper_bound(last_received);
        }
        HistoryMap<T> ret{std::move(requested_index), datas.upper_bound(frame_requested)};
        if (ret.size()) {
            auto it = ret.cend(); // make a copy
            last_received = (--it)->first;
        }
        return std::move(ret);
    };

    /** \brief Make the data available to all threads
     *
     * The futures are made ready
     *
     * \param data const T& the data to broadcast
     *
     */
    template<class U=const T>
    void Publish(U&& data, frame_tp frame) {
        std::unique_lock<std::mutex> locker(m_current);
        current_frame = std::move(frame);
        datas[current_frame] = std::forward<U>(data);
        datas.erase(datas.begin());
        ahead_request.notify_all();
    };

    /** \brief Get the data of the last frame available
     *
     * \return const T& the data
     *
     */
    const T& GetLastData() const {
        std::unique_lock<std::mutex> locker(m_current);
        return datas.at(current_frame);
    }

private:
    content_map datas;
    frame_tp current_frame;
    mutable std::mutex m_current;
    mutable std::condition_variable ahead_request;
};
} // namespace trillek

#endif // ASYNCDATA_HPP_INCLUDED
