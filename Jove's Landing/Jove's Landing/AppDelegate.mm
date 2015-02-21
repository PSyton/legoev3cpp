//
//  AppDelegate.m
//  Jove's Landing
//
//  Created by David Giovannini on 12/14/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#import "AppDelegate.h"

#include "SBJEV3ConnectionFactory.h"
#include "SBJEV3Brick.h"
#include "SBJEV3Log.h"
#import "RailSwitch.h"

#import "DiscoveredViewController.h"
#import "DeviceSelectionViewController.h"
#import "RailSwitchViewController.h"
#import "DirectoryListingViewController.h"

using namespace SBJ::EV3;

Log mylog(std::cout);

@interface AppDelegate ()
{
	std::unique_ptr<ConnectionFactory> _factory;
	std::unique_ptr<Brick> _brick;
	__weak DiscoveredViewController* _discovered;
	__weak DeviceSelectionViewController* _connectivity;
	__weak RailSwitchViewController* _rails;
	__weak DirectoryListingViewController* _dirListing;
	
}
@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
	// Override point for customization after application launch.
	
	__weak decltype(self) weakSelf = self;
	
	_factory.reset(new ConnectionFactory(mylog));
	_factory->start(^(DiscoveredDevice& device, DiscoveredDeviceChanged change)
	{
		[weakSelf updateUI];
	});
	
	_brick.reset(new Brick(*_factory));
	
	[RailSwitch installOnBrick: _brick.get()];
	
	UIViewController* root = self.window.rootViewController;
	[[root view] setAlpha: 1.0]; // hack to force load of view so we can get the child view controller loaded
	UITabBarController* controller = (UITabBarController*)root.childViewControllers[0];
	
	_discovered = controller.viewControllers[0];
	_connectivity = controller.viewControllers[1];
	_rails = controller.viewControllers[2];
	_dirListing = controller.viewControllers[3];
	
	[_discovered setConnectionFactory: _factory.get()];
	[_connectivity setBrick: _brick.get()];
	[_rails setBrick: _brick.get()];
	[_dirListing setBrick: _brick.get()];

	return YES;
}

- (void) updateUI
{
	[_discovered updateUI];
}


- (void)applicationWillResignActive:(UIApplication *)application {
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
