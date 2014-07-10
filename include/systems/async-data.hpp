#ifndef ASYNCDATA_HPP_INCLUDED
#define ASYNCDATA_HPP_INCLUDED

#include <mutex>
#include <iterator>
#include "trillek-scheduler.hpp"

namespace trillek {

template<class T,int HistorySize = 30>
class AsyncFrameData {
    typedef std::map<frame_tp,T> content_map;
public:

    typedef typename content_map::const_iterator iterator_type;
    typedef std::pair<iterator_type,iterator_type> return_type;

    AsyncFrameData() {
        for (auto i = 0; i < HistorySize; ++i) {
            datas.emplace_hint(datas.end(), std::make_pair<frame_tp,T>(frame_tp(frame_unit(i)), T()));
        }
    };

    /** \brief Return the frames between last_received and frame_requested
     *
     * last_received is not included.
     *
     * Actually return bound iterators on the map of data
     *
     * Blocks if the call is too early, and returns no data after 0.5s
     *
     * \param frame_requested const frame_tp& the "now" of the caller
     * \param last_received const frame_tp& the last frame received by the caller
     * \return return_type 2 iterators on the map of data for each frame
     *
     */
    return_type GetFramesData(const frame_tp& frame_requested, const frame_tp& last_received) const {
        std::unique_lock<std::mutex> locker(m_current);
        auto requested_index = datas.upper_bound(last_received);
        if (requested_index == datas.cend() || frame_requested > current_frame) {
            // we block until the frame time or after 500 ms
            if (! ahead_request.wait_for(locker, std::chrono::milliseconds(500), [&](){ return frame_requested <= current_frame; })) {
                // 500 ms have passed, let return a invalid future
                return { datas.cend(), datas.cend() };
            }
        }
        auto d = (current_frame - frame_requested);
        auto d2 = frame_requested - last_received;
        requested_index = datas.upper_bound(last_received);
        return { std::move(requested_index), datas.cend() };
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
    const T& GetLastFrameData() const {
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
