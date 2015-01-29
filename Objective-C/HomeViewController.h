//
//  HomeViewController.h
//
//  Created by Luming on 12-27-2014.
//  Copyright (c) 2014 Luming. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SecondViewController.h"
#import "ShoppingCartViewController.h"
#import "CollectinfoTabViewController.h"
#import "PersoninfoTabViewController.h"
#import "NotiveViewControll.h"
#import "PolyoreViewController.h"
@class ShoppingCartViewController;
@class MeViewController;

@interface HomeViewController : UIViewController
{

    ShoppingCartViewController *ShoppingCartView;

    MeViewController *TabView4;
    NSString * userid;
    IBOutlet UIView *tabbottomview;
    UIViewController *rightView; 
    UIView *leftView; 
    int top;
    
    UINavigationController *nav;
    UINavigationController *navP;
    UINavigationController *navS;
    IBOutlet UILabel *lab_login_title1;
    IBOutlet UILabel *lab_login_title2;
    IBOutlet UILabel *lab_login_title3;
    IBOutlet UIImageView *myimageface;
    IBOutlet UILabel *lab_login_name;
    
    UIView *searchview;
    IBOutlet UIButton *btn_return;
    
    IBOutlet UIButton *sofitButton;
    IBOutlet UIButton *favouriteButton;
    IBOutlet UIButton *groupButton;
    IBOutlet UIButton *homeButton;
    
    
}

@property (nonatomic, retain) IBOutlet UIButton *btn_return;
@property (nonatomic, assign, getter = isRefreshing) BOOL refreshing;
@property (nonatomic, retain)  UIView *searchview;
@property (nonatomic, retain) IBOutlet UIView *tabbottomview;
@property (nonatomic, retain) UINavigationController *navS;
-(IBAction)sofitClicked:(id)sender;
-(IBAction)favouriteClicked:(id)sender;
-(IBAction)homeClicked:(id)sender;
-(IBAction)groupClicked:(id)sender;
-(IBAction)takePhoto:(id)sender;
-(void)showHome;
-(void)languagechoose;
-(void)backlogin;
-(void)showset;
-(void)addchildview:(UIViewController *)conv;
-(void)removeView;
-(void)showPerson:(UIViewController *)vobj;
-(void)removeViewP;
-(void)showInitPage;
-(void)refresh;

@end
