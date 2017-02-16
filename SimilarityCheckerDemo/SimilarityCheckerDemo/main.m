#import <Foundation/Foundation.h>

@interface ArithOperation
- (void)mixOperation1;
- (void)mixOperation2;
@end

@implementation ArithOperation
- (NSInteger)mixOperation1:(NSInteger)n{
    NSInteger i = 1,j = 2;
    float m = 3.f;
    NSInteger r = (i+j)*m;
    if(n == 4){
        r = n*m;
    }
    return r;
}

- (NSInteger)mixOperation2:(NSInteger)n2{
    NSInteger i2 = 1;
    NSInteger j2 = 2;
    float m2 = 3.f;
    NSInteger r2 = (i2+j2)*m2;
    if(n2 == 4){
        r2 = n2*m2;
    }
    return r2;
}
@end

int main() {
    ArithOperation* operation = [ArithOperation new];
    [operation mixOperation1];
    [operation mixOperation2];
}
