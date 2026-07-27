#pragma once
#include <typeindex>
#include <concepts>

#include <vvhl/Events/Event.hpp>
#include <vvhl/Core/UUID.hpp>

namespace vvhl
{

using ListenerID = UUID;

class EventDispatcher{
public:

    template<typename EventType>
    ListenerID subscribe(std::function<void(const EventType&)> callback){
        ListenerID id;

        auto wrapper =
            [callback](const Event& event)
            {
                callback(static_cast<const EventType&>(event));
            };

        m_listeners[typeid(EventType)].push_back({
            id,
            wrapper
        });

        return id;
    }

    template<typename EventType>
    void unsubscribe(ListenerID id){

        auto it = m_listeners.find(typeid(EventType));

        if (it == m_listeners.end())
            return;

        auto& listeners = it->second;

        listeners.erase(
            std::remove_if(
                listeners.begin(),
                listeners.end(),
                [&](const Listener& l)
                {
                    return l.id == id;
                }),
            listeners.end());
    }

    template<typename EventType>
    requires std::derived_from<EventType, Event>
    void dispatch(const EventType& event){
        auto it = m_listeners.find(typeid(EventType));

        if (it == m_listeners.end())
            return;

        for (const auto& listener : it->second)
            listener.callback(event);
    }

private:

    struct Listener{
        ListenerID id;
        std::function<void(const Event&)> callback;
    };

    std::unordered_map<
        std::type_index,
        std::vector<Listener>
    > m_listeners;
};

}