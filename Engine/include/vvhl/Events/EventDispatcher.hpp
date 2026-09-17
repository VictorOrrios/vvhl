#pragma once

#include <algorithm>
#include <typeindex>
#include <deque>
#include <vvhl/Core/Logger.hpp>
#include <vvhl/Core/UUID.hpp>
#include <vvhl/Events/Event.hpp>

namespace vvhl {

using ListenerID = UUID;

class EventDispatcher {
public:
  EventDispatcher() = default;
  ~EventDispatcher()  = default;

  void destroy(){ 
    m_listeners.clear(); 
    std::deque<QueuedEvent> empty;
    std::swap(m_eventQueue, empty);
  }

  template <typename EventType>
  ListenerID subscribe(std::function<void(const EventType &)> callback) {
    ListenerID id;

    auto wrapper = [callback](const Event &event) {
      callback(static_cast<const EventType &>(event));
    };

    m_listeners[typeid(EventType)].push_back({id, wrapper});

    return id;
  }

  template <typename EventType> void unsubscribe(ListenerID id) {

    auto it = m_listeners.find(typeid(EventType));

    if (it == m_listeners.end())
      return;

    auto &listeners = it->second;

    listeners.erase(
        std::remove_if(listeners.begin(), listeners.end(),
                       [&](const Listener &l) { return l.id == id; }),
        listeners.end());
  }

  template <typename EventType>
    requires std::derived_from<EventType, Event>
  void dispatch(const EventType &event) {
    auto it = m_listeners.find(typeid(EventType));

    if (it == m_listeners.end())
      return;

    for (const auto &listener : it->second)
      listener.callback(event);
  }

  template <typename EventType>
    requires std::derived_from<EventType, Event>
  void enqueue(EventType event) {

    const auto type = std::type_index(typeid(EventType));

    if constexpr (EventType::Coalescable) {
      // If an event of this type is already pending,
      // replace it with the newest event.
      for (auto& queued : m_eventQueue) {
        if (queued.type == type) {
          queued.event = std::make_unique<EventType>(
            std::move(event)
          );

          return;
        }
      }
    }

    m_eventQueue.push_back({
      type,
      std::make_unique<EventType>(std::move(event))
    });
  }

  void poll() {
    std::deque<QueuedEvent> toProcess;
    std::swap(toProcess, m_eventQueue);

    while (!toProcess.empty()) {
      auto queued = std::move(toProcess.front());
      toProcess.pop_front();

      dispatchQueuedEvent(queued);
    }
  }

public:
  size_t pendingCount() const { return m_eventQueue.size(); }


private:
  struct Listener {
    ListenerID id;
    std::function<void(const Event &)> callback;
  };
  struct QueuedEvent {
    std::type_index type;
    std::unique_ptr<Event> event;
  };

private:
  void dispatchQueuedEvent(const QueuedEvent& queued) {
    auto it = m_listeners.find(queued.type);

    if (it == m_listeners.end())
      return;

    for (const auto& listener : it->second)
      listener.callback(*queued.event);
  }
  
private:
  std::unordered_map<std::type_index, std::vector<Listener>> m_listeners;
  std::deque<QueuedEvent> m_eventQueue;
};

} // namespace vvhl