#import "CXGChat.h"
#import "ChatManager.hpp"
#import "MessageControl.h"


@implementation Message

- (void) setUid:(NSString*) uid {
    mUid = uid;
}
- (void) setNickName:(NSString*) name {
    mNickName = name;
}
- (void) setMsg:(NSString*) msg {
    mMsg = msg;
}

- (void) setType:(NSInteger) type {
    mType = type;
}

- (NSString*) getUid {
    return mUid;
}
- (NSString*) getNickName {
    return mNickName;
}
- (NSString*) getMsg {
    return mMsg;
}
- (NSInteger) getType {
    return mType;
}

- (User*) getUser {
    return mUser;
}

- (Gift*) getGift {
    return mGift;
}

- (id) init
{
    if(self = [super init])
    {
        mUser = [[User alloc] init];
        mGift = [[Gift alloc] init];
    }
    return self; 
}

@end


@implementation User

- (NSInteger) getFensi {
    return fensi;
}

- (NSInteger) getCaifu {
    return caifu;
}

- (NSInteger) getJuese {
    return juese;
}

- (void) setFensi:(NSInteger) temp {
    self->fensi = temp;
}

- (void) setCaifu:(NSInteger) temp {
    self->caifu = temp;
}

- (void) setJuese:(NSInteger) temp {
    self->juese = temp;
}

@end

@implementation  Gift

- (NSString*) getName {
    return name;
}
- (NSInteger) getCount {
    return count;
}

- (void) setName:(NSString*) temp {
    self->name = temp;
}

- (void) setCount:(NSInteger) temp {
    self->count = temp;
}

@end




@interface ChatInterface ()

@property long long mUid;    // user id
@property unsigned long long mRid;

@property (nonatomic,strong)NSString* mToken;    // user id

@property (nonatomic,strong)NSString* mUname;    // user id

@property (nonatomic,assign)long mPort;    // user id
@property (nonatomic,assign)NSString* mHost;   // user id

@property (nonatomic,strong)ChatControl* mController; //

@end


@implementation ChatInterface

- (void)dealloc
{
    _mController = nil;
}

- (void) setRoomInfoWith:(CHATROOM_INFO) info
{
    ChatManager* manager = ChatManager::GetInstance();
    if(manager != NULL)
    {
        manager->setChatRoomInfo(info);
    }
    
    if (!_mController) {
        _mController = [[ChatControl alloc] init];
    }
}

- (int) EnterRoom
{
    NSString* resources = [[NSBundle mainBundle] resourcePath];
    NSString *path = [NSString stringWithFormat:@"%s/Resource/locale/", [resources cStringUsingEncoding:NSUTF8StringEncoding]] ;
    setenv( "PATH_LOCALE", [path UTF8String] ,1);
    setlocale(LC_CTYPE, "zh_CN.UTF-8");
    
    ChatManager* manager = ChatManager::GetInstance();
    manager->setController(_mController);
    manager->EnterChatRoom();

    return 1;
}

- (int) LeaveRoom
{
    ChatManager* manager = ChatManager::GetInstance();
    if(manager != NULL)
        manager->ExitChatRoom();
    
    return 1;
}

-(void) SendMsgWithContent:(NSString *)Msg andToUser:(unsigned long long)uid andisPriavte:(bool) bPrivate
{
    ChatManager* manager = ChatManager::GetInstance();
    if(manager != NULL)
        manager->SendChatMsg(Msg, uid, bPrivate);
}

- (void)setReceiverObject:(id<OnChatDelegate>)delegate
{
    _mController.delegate = delegate;
}




@end
