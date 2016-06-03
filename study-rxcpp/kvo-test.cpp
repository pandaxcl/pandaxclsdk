#include <rxcpp/rx.hpp>
#include "kvo_extension.hpp"

struct Message
{
    Message(bool isRead):isRead(isRead) {}
    kvo_property<bool> isRead = false;// 是否已读
};

struct Conversation
{
    kvo_container<std::shared_ptr<Message>> messages;
    kvo_property<int> unreadCount = 0;// 未读消息数量
	Conversation() 
	{
        rxcpp::observable<>::from(messages.subject_setting.get_observable(),
                                  messages.subject_insertion.get_observable(),
                                  messages.subject_removal.get_observable())
        .merge()
        .map([this](const auto&){
            return rxcpp::observable<>::iterate(this->messages.get())
            .map([](const auto message){
                return message->isRead.subject.get_observable();
            })
            .merge()
            //.filter([](bool x){ return false == x; })
            //.count()
            ;
        })
        .switch_on_next()
        //.filter([](bool x){ return false == x; })
        //.count()
        .subscribe([this](const auto&x){
            int nCount = 0;
            for (auto m:this->messages.get())
            {
                if (false == m->isRead.get())
                {
                    nCount ++;
                }
            }
            this->unreadCount.set(nCount);
            std::cout<<"unreadCount = "<< this->unreadCount.get() << ", x = " << x << std::endl;
        }, [](std::exception_ptr) {}, [](){})
        ;
	}
    
    void addMessage(const std::shared_ptr<Message>&x)
    {
        messages.insert({x});
    }
    
//    void removeMessage(const Message&x)
//    {
//        messages.remove({x});
//    }
};

int main(int argc, const char*argv[])
{
    auto message_0 = std::make_shared<Message>(true);
    auto message_1 = std::make_shared<Message>(false);
    auto message_2 = std::make_shared<Message>(true);
    auto message_3 = std::make_shared<Message>(false);
    auto message_4 = std::make_shared<Message>(true);
    
    kvo_container<std::shared_ptr<Message>> all_messages;
    all_messages.insert({message_0,message_1,message_2,message_3,message_4});
    
    Conversation conversation;
    conversation.addMessage(message_0);
    conversation.addMessage(message_1);
    conversation.addMessage(message_4);
    
    message_1->isRead.set(true);
    message_1->isRead.set(false);
	std::cout<<"last unreadCount = "<< conversation.unreadCount.get() << std::endl;
	return 0;
}
