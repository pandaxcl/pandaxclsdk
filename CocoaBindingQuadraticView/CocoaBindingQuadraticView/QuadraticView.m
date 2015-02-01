//
//  QuadraticView.m
//  CocoaBindingQuadraticView
//
//  Created by 熊 春雷 on 15/2/1.
//  Copyright (c) 2015年 pandaxcl. All rights reserved.
//

#import "QuadraticView.h"
@interface QuadraticView()
@property(assign) CGFloat width;
@property(assign) CGFloat height;
@end

@implementation QuadraticView

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    NSLog(@"%s", __PRETTY_FUNCTION__);
    // Drawing code here.
    self.layer.backgroundColor = (__bridge CGColorRef)([NSColor redColor]);
//    self.width = 200;
//    self.height = 100;
    NSRect rect = NSMakeRect(NSMidX(self.bounds), NSMidY(self.bounds), self.width, self.height);
    [NSBezierPath strokeRect:rect];
}

#pragma mark - Cocoa Binding Support

-(void)bind:(NSString *)binding toObject:(id)observable withKeyPath:(NSString *)keyPath options:(NSDictionary *)options
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    BOOL hasProcessed = NO;
    
    if ([binding isEqualToString:@"width"]) {
        hasProcessed = YES;
        NSKeyValueObservingOptions options = NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld;
        [observable addObserver:self forKeyPath:keyPath options:options context:(__bridge void*)@"width"];
    }
    
    if ([binding isEqualToString:@"height"]) {
        hasProcessed = YES;
        NSKeyValueObservingOptions options = NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld;
        [observable addObserver:self forKeyPath:keyPath options:options context:(__bridge void*)@"height"];
    }
    if (hasProcessed)
    {
        
    }
    else
    {
        [super bind:binding toObject:observable withKeyPath:keyPath options:options];
    }
}

-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    BOOL hasProcessed = NO;
    
    if ([(__bridge NSString*)context isEqualToString:@"width"]) {
        hasProcessed = YES;
        self.width = [[object valueForKeyPath:keyPath] doubleValue];
        [self setNeedsDisplay:YES];
    }
    
    if ([(__bridge NSString*)context isEqualToString:@"height"]) {
        hasProcessed = YES;
        self.height = [[object valueForKeyPath:keyPath] doubleValue];
        [self setNeedsDisplay:YES];
    }
    if (hasProcessed)
    {
        
    }
    else
    {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
    }
    
}

@end
