#ifndef ASYNCDATA_HPP_INCLUDED
#define ASYNCDATA_HPP_INCLUDED

#include <mutex>
#include <iterator>
#include "trillek-scheduler.hpp"

namespace trillek {

template<class T>
class HistoryMap {
public:
    HistoryMap(typename std::map<frame_tp,T>::const_iterator&& begin,
                typename std::map<frame_tp,T>::const_iterator&& end,
                frame_tp& last_update)
        : start(std::move(begin)), stop(std::move(end)), hsize(std::distance(start, stop))
        {
            SetLastUpdatePoint(last_update);
        };

    typename std::map<frame_tp,T>::const_iterator& cbegin() {
        return start;
    }

    typename std::map<frame_tp,T>::const_iterator& cend() {
        return stop;
    }

    typename std::map<frame_tp,T>::size_type size() const {
        return hsize;
    }

    const frame_tp& OldRetrievedPoint() const {
        return old_last_point;
    }

private:
    void SetLastUpdatePoint(frame_tp& last) {
        if (size() > 0) {
            auto it = cend(); // make a copy
            old_last_point = (--it)->first;
            swap(old_last_point, last);
            return;
        }
    }

    typename std::map<frame_tp,T>::const_iterator start;
    typename std::map<frame_tp,T>::const_iterator stop;
    typename std::map<frame_tp,T>::size_type hsize;
    frame_tp old_last_point;
};

template<class T>
class ReverseHistoryMap {
public:
    ReverseHistoryMap(typename std::map<frame_tp,T>::const_iterator&& last,
                typename std::map<frame_tp,T>::const_iterator&& first)
        : start(std::move(last)), stop(std::move(first)), hsize(std::distance(stop, start))
        {};

    typename std::map<frame_tp,T>::const_iterator& crbegin() {
        return start;
    }

    typename std::map<frame_tp,T>::const_iterator& crend() {
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
        for (auto i = -HistorySize; i < 0; ++i) {
            datas.emplace_hint(datas.end(), std::make_pair<frame_tp,T>(frame_tp(frame_unit(i)), T()));
        }
    };

    /** \brief Return the frames between last_received and frame_requested
     *
     * last_received is not included, frame_requested may be included.
     *
     * last_received will be updated if some data is retrieved
     *
     * Blocks if the call is before the publication of new data since last_received,
     * and returns no data after 0.01s
     *
     * \param frame_requested const frame_tp& the "now" of the caller
     * \param last_received frame_tp& the last frame received by the caller
     * \return HistoryMap An object that with begin(), end() and size() functions
     *
     */
    HistoryMap<T> GetHistoryData(const frame_tp& frame_requested, frame_tp& last_received) const {
        std::unique_lock<std::mutex> locker(m_current);
        auto requested_index = datas.upper_bound(last_received);
        if (requested_index == datas.cend()) {
            // we block until next publication or after 10 ms
            if (! ahead_request.wait_for(locker, std::chrono::milliseconds(10), [&](){ return requested_index != datas.cend(); })) {
                // 10 ms have passed, let return a empty object
                return HistoryMap<T>(datas.cend(), datas.cend(), last_received);
            }
            requested_index = datas.upper_bound(last_received);
        }
        return HistoryMap<T>{std::move(requested_index), datas.upper_bound(frame_requested), last_received};
    };

    /** \brief Return the frames between first and last frame in reverse order
     *
     * first is never included
     * last is always included
     *
     * The object returned give reverse iterators from last to first.
     *
     * This allows to return in a previous state.
     *
     * Blocks if the call is before last_frame, and returns no data after 0.01s
     *
     * \param first_frame const frame_tp& the first frame
     * \param last_frame const frame_tp& the last frame
     * \return ReverseHistoryMap<T> the iterable frames
     *
     */
    ReverseHistoryMap<T> GetReverseHistoryData(const frame_tp& first_frame, const frame_tp& last_frame) const {
        std::unique_lock<std::mutex> locker2(m_current2);
        auto requested_index = datas.lower_bound(last_frame);
        if (requested_index == datas.cend() || last_frame > current_frame) {
            // we block until the frame time or after 10 ms
            if (! ahead_request2.wait_for(locker2, std::chrono::milliseconds(10), [&](){ return last_frame <= current_frame; })) {
                // 10 ms have passed, let return a empty object
                return ReverseHistoryMap<T>(datas.cend(), datas.cend());
            }
            requested_index = datas.lower_bound(last_frame);
        }
        return ReverseHistoryMap<T>{std::move(requested_index), datas.lower_bound(first_frame)};
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
        std::unique_lock<std::mutex> locker2(m_current2);
        current_frame = std::move(frame);
        datas[current_frame] = std::forward<U>(data);
        datas.erase(datas.begin());
        ahead_request.notify_all();
        ahead_request2.notify_all();
    };

    /** \brief Get the data of the last frame available
     *
     * \return const T& the data
     *
     */
    const T& GetLastData() const {
        std::unique_lock<std::mutex> locker(m_current);
        std::unique_lock<std::mutex> locker2(m_current2);
        return datas.at(current_frame);
    }

private:
    content_map datas;
    frame_tp current_frame;
    mutable std::mutex m_current;
    mutable std::mutex m_current2;
    mutable std::condition_variable ahead_request;
    mutable std::condition_variable ahead_request2;
};
} // namespace trillek

#endif // ASYNCDATA_HPP_INCLUDED
