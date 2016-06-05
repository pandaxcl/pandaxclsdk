#include <rxcpp/rx.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "kvo_extension.hpp"

struct User
{
    kvo_property<std::string> name;
    kvo_property<std::string> icon;
    kvo_property<int> age;
};

struct Message
{
    Message() = default;
    Message(bool isRead):isRead(isRead) {}
    kvo_property<bool> isRead = false;// 是否已读
    kvo_property<std::string> content = std::string("");
    kvo_property<std::shared_ptr<User>> fromUser = std::shared_ptr<User>(nullptr);
};

struct Conversation
{
    kvo_container<std::shared_ptr<Message>> messages;
    kvo_property<int> unreadCount = 0;// 未读消息数量
    kvo_property<std::shared_ptr<Message>> lastMessageFromOther = std::make_shared<Message>();
	Conversation() 
	{
        rxcpp::observable<>::from(messages.subject_setting.get_observable(),
                                  messages.subject_insertion.get_observable(),
                                  messages.subject_removal.get_observable())
        .merge()
        .map([this](const auto&){
            return rxcpp::observable<>::iterate(this->messages.get())
            .map([](const auto x){ return x->isRead.subject.get_observable(); })
            .merge()
            ;
        })
        .merge()
        .subscribe([this](const auto&){
            int n = 0;
            for (auto m:this->messages.get())
                n += false == m->isRead.get();
            this->unreadCount.set(n);
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

class Manager
{
    kvo_container<std::shared_ptr<User>> users;
    kvo_container<std::shared_ptr<Message>> messages;
    
    kvo_container<std::shared_ptr<Conversation>> conversations;
    kvo_container<std::shared_ptr<Conversation>> topConversations;
    kvo_container<std::shared_ptr<Conversation>> normalConversations;
};

struct ConversationListViewModel
{
    kvo_container<std::shared_ptr<Conversation>> conversations;
    void buttonDeleteClicked(std::shared_ptr<Conversation>);
};

struct ConversationCellViewModel
{
    kvo_property<std::shared_ptr<Message>> message;
    kvo_property<std::string> nickName;
    kvo_property<std::string> userIcon;
    kvo_property<int> unreadCount;
};

struct ConversationMessageViewModel
{
    kvo_container<std::shared_ptr<Message>> messages;
    void textFieldContentChanged(std::string);
    void buttonSendClicked();
};

struct ConversationConfigViewModel
{
    kvo_property<bool> isTop;
    kvo_property<bool> isMask;
    void switchTopStatusChanged(bool isTop);
    void switchMaskStatusChanged(bool isMask);
    void buttonClearClicked();
};


TEST_CASE("未读消息数量")
{
    SECTION("")
    {
        auto message_0 = std::make_shared<Message>(true);
        auto message_1 = std::make_shared<Message>(false);
        auto message_2 = std::make_shared<Message>(true);
        auto message_3 = std::make_shared<Message>(false);
        auto message_4 = std::make_shared<Message>(true);
        
        REQUIRE(message_0->isRead.get() == true);
        REQUIRE(message_1->isRead.get() == false);
        REQUIRE(message_2->isRead.get() == true);
        REQUIRE(message_3->isRead.get() == false);
        REQUIRE(message_4->isRead.get() == true);
        
        kvo_container<std::shared_ptr<Message>> all_messages;
        all_messages.insert({message_0,message_1,message_2,message_3,message_4});
        
        REQUIRE(all_messages.get().size() == 5);
        
        Conversation conversation;
        REQUIRE(conversation.unreadCount.get() == 0);
        conversation.addMessage(message_0);
        REQUIRE(conversation.unreadCount.get() == 0);
        conversation.addMessage(message_1);
        REQUIRE(conversation.unreadCount.get() == 1);
        conversation.addMessage(message_4);
        REQUIRE(conversation.unreadCount.get() == 1);
        
        message_1->isRead.set(true);
        REQUIRE(conversation.unreadCount.get() == 0);
        message_1->isRead.set(false);
        REQUIRE(conversation.unreadCount.get() == 1);
        
        message_4->isRead.set(true);
        REQUIRE(conversation.unreadCount.get() == 1);
        message_4->isRead.set(false);
        REQUIRE(conversation.unreadCount.get() == 2);
    }
}

