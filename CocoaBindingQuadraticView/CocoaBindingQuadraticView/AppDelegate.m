//
//  AppDelegate.m
//  CocoaBindingQuadraticView
//
//  Created by 熊 春雷 on 15/2/1.
//  Copyright (c) 2015年 pandaxcl. All rights reserved.
//

#import "AppDelegate.h"
#import "QuadraticView.h"

@interface AppDelegate ()
@property (weak) IBOutlet NSWindow *window;

@property (weak) IBOutlet QuadraticView *quadraticView;
@property (weak) IBOutlet NSSlider *sliderWidth;
@property (weak) IBOutlet NSSlider *sliderHeight;
@property (weak) IBOutlet NSTextField *textFieldWidth;
@property (weak) IBOutlet NSTextField *textFieldHeight;

@property (strong) IBOutlet NSObjectController *objectController;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    NSDictionary*dict = @{@"width":@200.0, @"height":@100.0};
    self.objectController.content = [[NSMutableDictionary alloc] initWithDictionary:dict];
    
    [self.sliderWidth bind:@"value" toObject:self.objectController withKeyPath:@"content.width" options:@{}];
    [self.sliderHeight bind:@"value" toObject:self.objectController withKeyPath:@"content.height" options:@{}];
    [self.textFieldWidth bind:@"value" toObject:self.objectController withKeyPath:@"content.width" options:@{}];
    [self.textFieldHeight bind:@"value" toObject:self.objectController withKeyPath:@"content.height" options:@{}];
    
    [self.quadraticView bind:@"width" toObject:self.objectController withKeyPath:@"content.width" options:@{}];
    [self.quadraticView bind:@"height" toObject:self.objectController withKeyPath:@"content.height" options:@{}];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
