#import "MessageControl.h"

@interface ChatControl ()
{
    dispatch_queue_t chatQueue;
}
@end


@implementation ChatControl

- (void)dealloc
{
}

- (id)init
{
    if (self = [super init])
    {
        chatQueue = dispatch_queue_create([@"ShowRoom" UTF8String], DISPATCH_QUEUE_SERIAL);
    }
    return self;
}

- (void) doOnMessage :(Message*) pkg{
    
    dispatch_async(chatQueue, ^{
        
        if (_delegate && [_delegate respondsToSelector:@selector(onMessage:)]) {
            [self.delegate onMessage:pkg];
        }
    });
}

- (void) doOnError :(int) errorCode Msg:(NSString*) msg{
    
    dispatch_async(chatQueue, ^{
        if (_delegate && [_delegate respondsToSelector:@selector(onError:Msg:)]) {
            [self.delegate onError:errorCode Msg:msg];
        }
    });
}

- (void) doOnChatRoom : (int) msgCode Msg:(NSString*) msg{
    dispatch_async(chatQueue, ^{
        if(_delegate && [_delegate respondsToSelector:@selector(doOnChatRoom:Msg:)]){
            [self.delegate onChatRoom:msgCode Msg:msg];
        }
    });
}

@end
