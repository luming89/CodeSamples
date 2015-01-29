//
//  viewHome.m
//  VISS
//
//  Created by 欢欢 范 on 14-6-23.
//  Improved by Luming on Jan. 4, 2015
//  Copyright (c) 2014年 tiger. All rights reserved.
//

#import "HomeViewController.h"
#import "AppDelegate.h"
//#import "MCTakePic.h"
#import "LeGlobalObj.h"
#import "LoginViewController.h"
//#import "YMAPIReader3.h"
//#import "Config.h"
//#import "PolyoreViewController.h"             // already imported in .h?
#import "FirstPageViewController.h"
//#import "PAImageView.h"
//#import "UIImageView+WebCache.h"
//#import "SecondDetailViewController.h"
#import "ShoppingCartViewController.h"
#import "MeViewController.h"    // already imported in .h?
//#import "SecondViewController2.h"               // WTF is this? Do we have 2 SecondViewController.h???
#import "Macros.h"

@interface HomeViewController ()

@end

@implementation HomeViewController
@synthesize tabbottomview;
@synthesize searchview,navS,refreshing,btn_return;
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
#if DEBUG
    NSLog(@"HomeViewNib%@",nibNameOrNil);
#endif
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    //CGSize size=[[UIScreen mainScreen]bounds].size;
    //CGFloat screenHeight=size.height;

    
    NSString *filename1=[[LeGlobalObj sharedSingleton] GetFileName:@"username.txt"];
    if (![[LeGlobalObj sharedSingleton] is_file_exist:filename1])
    {
        //FirstPageViewController *frmobj=[[FirstPageViewController alloc] init]; // WTF is this?
        ///[self.view addSubview:frmobj.view];
        //frmobj.view.frame=self.view.frame;
    }
    //AppDelegate *appdelegate=[[UIApplication sharedApplication]delegate];
    //userid=@"1";
    //appdelegate.loginuserid=@"1";
    //appdelegate.loginusername=@"请登录";
    //appdelegate.loginavatar=@"1";
    //[LeGlobalObj sharedSingleton].userid=@"1";
    [self showInitPage];
    [self autologin];

  
////    [btn1 setBackgroundImage:[UIImage imageNamed:@"in1_white"] forState:UIControlStateNormal];
    //[LeGlobalObj sharedSingleton].HomePage=self;
    //TabView1=[[PersoninfoTabViewController alloc] init];
//    
    ShoppingCartView=[[ShoppingCartViewController alloc] init];
//    TabView3=[[PolyoreViewController alloc] init];
    TabView4=[[MeViewController alloc] init];
    TabView4.userid=userid;
//
//    TabHome=[[SecondViewController alloc] init];
     if (false)//isipad)
     {
//    TabHome.tabbottomview=self.tabbottomview;
//    TabView1.personbottomview=self.tabbottomview;
    ShoppingCartView.shopcarbottomview=self.tabbottomview;
    TabView4.tabbottomview=self.tabbottomview;
//     
//         TabView3.bottomview=self.tabbottomview;
     }
     else
     {
//         
         //CGRect frame1=CGRectMake(148, 0, 320, 768);
         CGRect frame1=CGRectMake(0, 0, 320, 768);
//         TabHome.view.frame=frame1;
//         TabView1.view.frame=frame1;
          ShoppingCartView.view.frame=frame1;
//          TabView3.view.frame=frame1;
         TabView4.view.frame=frame1;
     }
//    [self.view addSubview:TabView1.view];
    [self.view addSubview:ShoppingCartView.view];
// 
//    [self.view addSubview:TabView3.view];
    [self.view addSubview:TabView4.view];
//    [self.view addSubview:TabHome.view];
//    
//   
//    [TabView1.view setHidden:YES];
    [ShoppingCartView.view setHidden:YES];
//   [TabView3.view setHidden:YES];
    [TabView4.view setHidden:YES];
//    
//     
//    [self.view bringSubviewToFront:self.tabbottomview];
//    [self.view sendSubviewToBack:TabView1.view];
    [self.view sendSubviewToBack:ShoppingCartView.view];
//    [self.view sendSubviewToBack:TabView3.view];
    [self.view sendSubviewToBack:TabView4.view];
//    top=0;
//
// 
//    [[LeGlobalObj sharedSingleton].SetPage.view setHidden:YES];
//
//    lab_login_title1.text=@"";
//    
//    lab_login_title3.text=@"";
//    lab_login_name.text=@"";
//    
//    [myimageface setHidden:YES];
////    [myimageface setHidden:YES];
//
//    NSString *filename1=[[LeGlobalObj sharedSingleton] GetFileName:@"username.txt"];
//    if (![[LeGlobalObj sharedSingleton] is_file_exist:filename1]) {
//        FirstPageViewController *frmobj=[[FirstPageViewController alloc] init];
//        [self.view addSubview:frmobj.view];
//        frmobj.view.frame=self.view.frame;
//    }

 
    // Do any additional setup after loading the view from its nib.
}
//
//- (UIStatusBarStyle)preferredStatusBarStyle
//{
//    return UIStatusBarStyleLightContent;
//}

-(void)backlogin{
    FirstPageViewController *frmobj=[[FirstPageViewController alloc] init];
    [self.view addSubview:frmobj.view];
    [self showHome];
    
    lab_login_title1.text=@"";
    
    lab_login_title3.text=@"";
    lab_login_name.text=@"";
    
    [myimageface setHidden:YES];
    
}
-(void)languagechoose
{
     //[TabHome languagechoose];
    //[TabView1 languagechoose];
      //[TabView2 languagechoose];
 
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
-(IBAction)sofitClicked:(id)sender
{
#if DEBUG
    NSLog(@"Sofit clicked");
#endif
    if (leftView!=nil)
    {
        [leftView removeFromSuperview];
    }
    
    if (searchview!=nil)
    {
        [searchview removeFromSuperview];
    }
    /*[[LeGlobalObj sharedSingleton].SetPage.view removeFromSuperview];
    [TabView1.view setHidden:YES];*/
    [ShoppingCartView.view setHidden:YES];
    //[TabView3.view setHidden:YES];
    [TabView4.view setHidden:YES];
    //[TabHome.view setHidden:NO];
    
    [sofitButton setBackgroundImage:[UIImage imageNamed:@"sofit_orange"] forState:UIControlStateNormal];
    [favouriteButton setBackgroundImage:[UIImage imageNamed:@"favourite_gray"] forState:UIControlStateNormal];
    [groupButton setBackgroundImage:[UIImage imageNamed:@"group_gray"] forState:UIControlStateNormal];
    [homeButton setBackgroundImage:[UIImage imageNamed:@"home_gray"] forState:UIControlStateNormal];
}
-(IBAction)favouriteClicked:(id)sender
{
#if DEBUG
    NSLog(@"Favourite clicked");
#endif
    if (leftView!=nil) {
        [leftView removeFromSuperview];
    }
    
    if (searchview!=nil) {
        [searchview removeFromSuperview];
    }
    /*[[LeGlobalObj sharedSingleton].SetPage.view removeFromSuperview];
    [TabView1.view setHidden:YES];*/
    [ShoppingCartView.view setHidden:NO];
    //[TabView3.view setHidden:YES];
    [TabView4.view setHidden:YES];
    //[TabHome.view setHidden:YES];

    [sofitButton setBackgroundImage:[UIImage imageNamed:@"sofit_gray"] forState:UIControlStateNormal];
    [favouriteButton setBackgroundImage:[UIImage imageNamed:@"favourite_orange"] forState:UIControlStateNormal];
    [groupButton setBackgroundImage:[UIImage imageNamed:@"group_gray"] forState:UIControlStateNormal];
    [homeButton setBackgroundImage:[UIImage imageNamed:@"home_gray"] forState:UIControlStateNormal];
}
-(IBAction)groupClicked:(id)sender
{
#if DEBUG
    NSLog(@"Group clicked");
#endif
    NSString *userId1=[LeGlobalObj sharedSingleton].userId;
    if([userId1 isEqualToString:@""])
    {
        
        LoginViewController *lvc=[[LoginViewController alloc] init];
        [self presentViewController:lvc animated:YES completion:^{
            
        }];
        //return;
        
    }
    else
    {
        [sofitButton setBackgroundImage:[UIImage imageNamed:@"sofit_gray"] forState:UIControlStateNormal];
        [favouriteButton setBackgroundImage:[UIImage imageNamed:@"favourite_gray"] forState:UIControlStateNormal];
        [groupButton setBackgroundImage:[UIImage imageNamed:@"group_orange"] forState:UIControlStateNormal];
        [homeButton setBackgroundImage:[UIImage imageNamed:@"home_gray"] forState:UIControlStateNormal];
        
        /*if (leftView!=nil) {
         [leftView removeFromSuperview];
         
         }
         if (searchview!=nil) {
         [searchview removeFromSuperview];
         }
         [[LeGlobalObj sharedSingleton].SetPage.view removeFromSuperview];
         [TabView1.view setHidden:NO];
         [TabView1 biaoqian1];*/
         [ShoppingCartView.view setHidden:YES];
         //[TabView3.view setHidden:YES];
         [TabView4.view setHidden:YES];
         //[TabView4 setUserid:userid];
         
         //[TabHome.view setHidden:YES];
    }
}

-(IBAction)homeClicked:(id)sender
{
#if DEBUG
    NSLog(@"Home clicked");
#endif
    
    //NSString *userId1=[LeGlobalObj sharedSingleton].userId;
    if(false)//[userId1 isEqualToString:@""])
    {
        
        LoginViewController *lvc=[[LoginViewController alloc] init];
        [self presentViewController:lvc animated:YES completion:^{}];
        //return;
    }
    else
    {
    //[[LeGlobalObj sharedSingleton].SetPage.view removeFromSuperview];
    //[TabView1.view setHidden:YES];
        [ShoppingCartView.view setHidden:YES];
    //[TabView3.view setHidden:YES];
        [TabView4.view setHidden:NO];
        [TabView4 one_jiazai];
    //[TabHome.view setHidden:YES];
        [sofitButton setBackgroundImage:[UIImage imageNamed:@"sofit_gray"] forState:UIControlStateNormal];
        [favouriteButton setBackgroundImage:[UIImage imageNamed:@"favourite_gray"] forState:UIControlStateNormal];
        [groupButton setBackgroundImage:[UIImage imageNamed:@"group_gray"] forState:UIControlStateNormal];
        [homeButton setBackgroundImage:[UIImage imageNamed:@"home_orange"] forState:UIControlStateNormal];
    }
    
}


-(void)showHome
{
    if (leftView!=nil) {
        [leftView removeFromSuperview];
    }
    if (searchview!=nil) {
        [searchview removeFromSuperview];
    }
    
    /*[[LeGlobalObj sharedSingleton].SetPage.view removeFromSuperview];
    [TabView1.view setHidden:YES];*/
    [ShoppingCartView.view setHidden:YES];
    //[TabView3.view setHidden:YES];
    [TabView4.view setHidden:YES];
    //[TabHome.view setHidden:NO];
}
-(void)viewDidAppear:(BOOL)animated
{

    //[TabView4 one_jiazai];
}

-(IBAction)takePhoto:(id)sender
{
#if DEBUG
    NSLog(@"takePhoto Button clicked");
#endif
    
    if ([LeGlobalObj sharedSingleton].userinfo ==nil)
    {
        LoginViewController *login=[[LoginViewController alloc]init];
        [login setfrom:@"other"];
        [self presentViewController:login animated:YES completion:^{
            
        }];
        
        //return;
    }
    //MCTakePic *frmobj=[[MCTakePic alloc] init];
    
    //[self.navigationController pushViewController:frmobj animated:YES];
}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView{
 
    
  
    if (scrollView.contentOffset.y<0) {
 
        return;
    }
    //    //NSLog(@"=== top:%d,now:%f=====",top,scrollView.contentOffset.y);
    if (top<scrollView.contentOffset.y) {
        
        
    
    }else{
 
    }
    
    top=scrollView.contentOffset.y;
    
    
    
}
-(void)autologin
{
    /*NSString *filename1=[[LeGlobalObj sharedSingleton] GetFileName:@"username.txt"];
    if (![[LeGlobalObj sharedSingleton] is_file_exist:filename1]) {\
        
        if ([LeGlobalObj sharedSingleton].userinfo ==nil) {
//            LoginViewControll *login=[[LoginViewControll alloc]init];
//            [login setfrom:@"other"];
//           
//            [self presentViewController:login animated:YES completion:^{
//                
//            }];
            
            return;
        }
    }
    NSString *savestr=[NSString stringWithContentsOfFile:filename1 encoding:NSUTF8StringEncoding error:nil];
    NSArray *arr=[savestr componentsSeparatedByString:@"|"];
    NSString *loginname=arr[0];
    NSString *loginpassword=arr[1];
    NSString *token=  [LeGlobalObj sharedSingleton].tokenStr;
    NSDictionary *dict=@{@"fun":@"login",
                         @"username":loginname,
                         @"password":loginpassword,
                         @"token":token
                         };
    
    YMAPIReader3 *reader=[[YMAPIReader3 alloc]init];
    [reader readWithAPIBaseURLString:Viss_server_url queryDictionary:dict failureComplation:^(NSError *__autoreleasing *error){
    } successComplation:^(NSInteger status,NSString *function,NSString *message,NSArray *content){
        
        
        if ([content count]>0) {
            NSMutableDictionary *dict=[content objectAtIndex:0];
            [LeGlobalObj sharedSingleton].userinfo=dict;
             [LeGlobalObj sharedSingleton].userid=dict[@"id"];
            AppDelegate *appdelegate=[[UIApplication sharedApplication]delegate];
            userid=dict[@"id"];
            appdelegate.loginuserid=dict[@"id"];
            appdelegate.loginusername=dict[@"name"];
            appdelegate.loginavatar=dict[@"avatar"];
            [LeGlobalObj sharedSingleton].userid=dict[@"id"];
            [LeGlobalObj sharedSingleton].pwd=dict[@"password"];
            [self showface];
            //            viewHome *second=[[viewHome alloc]init];
            //            [self.navigationController pushViewController:second animated:YES];
        }
        
        
    }];*/
}

-(void)showInitPage
{
#if DEBUG
    NSLog(@"In showInitPage");
#endif
    //CGSize size=[[UIScreen mainScreen]bounds].size;
    //CGFloat screenHeight=size.height;
    
 
    [sofitButton setBackgroundImage:[UIImage imageNamed:@"sofit_orange"] forState:UIControlStateNormal];
    [LeGlobalObj sharedSingleton].HomeViewControllerPtr=self;
    //TabView1=[[PersonInfoTabViewController alloc] init];
    
    //TabView2=[[ShopcarViewController alloc] init];
//    TabView3=[[PolyoreViewController alloc] init];
    /*TabView4=[[PersoninfosecondViewController alloc] init];
    TabView4.key=@"2";
    TabView4.userid=userid;
    [TabView4 setUserid:userid];
    TabHome=[[SecondViewController alloc] init];
//    if (isipad) {
        TabHome.tabbottomview=self.tabbottomview;
        TabView1.personbottomview=self.tabbottomview;
        TabView2.shopcarbottomview=self.tabbottomview;
        TabView4.tabbottomview=self.tabbottomview;
        
        TabView3.bottomview=self.tabbottomview;
//    }else{
    
//        CGRect frame1=CGRectMake(148, 0, 320, 768);
//        TabHome.view.frame=frame1;
//        TabView1.view.frame=frame1;
//        TabView2.view.frame=frame1;
//        TabView3.view.frame=frame1;
//        TabView4.view.frame=frame1;
////    }
    [self.view addSubview:TabView1.view];
    [self.view addSubview:TabView2.view];
    
    [self.view addSubview:TabView3.view];
    [self.view addSubview:TabView4.view];
    [self.view addSubview:TabHome.view];
    
    
    [TabView1.view setHidden:YES];
    [TabView2.view setHidden:YES];
    [TabView3.view setHidden:YES];
    [TabView4.view setHidden:YES];
    
    
    [self.view bringSubviewToFront:self.tabbottomview];
    [self.view sendSubviewToBack:TabView1.view];
    [self.view sendSubviewToBack:TabView2.view];
    [self.view sendSubviewToBack:TabView3.view];
    [self.view sendSubviewToBack:TabView4.view];
    top=0;
    
    
    [[LeGlobalObj sharedSingleton].SetPage.view setHidden:YES];
    
    lab_login_title1.text=@"";
    
    lab_login_title3.text=@"";
    lab_login_name.text=@"";
    
    [myimageface setHidden:YES];
    //    [myimageface setHidden:YES];
    


    
    if ([LeGlobalObj sharedSingleton].userinfo==nil) {
        return;
    }
    
    NSString *username=[[LeGlobalObj sharedSingleton].userinfo objectForKey:@"name"];
    NSString *title=[NSString stringWithFormat:@"尊敬的%@",username];
    lab_login_title1.text=title;
    
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    //设定时间格式,这里可以设置成自己需要的格式
    [dateFormatter setDateFormat:@"yyyy-MM-dd"];
    //用[NSDate date]可以获取系统当前时间
    NSString *currentDateStr = [dateFormatter stringFromDate:[NSDate date]];
    //输出格式为：2010-10-27 10:22:13
    //                NSLog(@"%@",currentDateStr);
    
    lab_login_title3.text=[NSString stringWithFormat:@"你于%@登录",currentDateStr];
    lab_login_name.text=username;
    
    NSString *face=[[LeGlobalObj sharedSingleton].userinfo  objectForKey:@"avatar"];
    if (![face isEqualToString:@""]) {
        
        NSString *urlStr=[Viss_imagedown_server_url stringByAppendingString:face];
        urlStr=[urlStr stringByAppendingString:@"_small.jpg"];
        UIColor *faclor=[UIColor colorWithRed:255.0f/255.0f green:255.0f/255.0f blue:255.0f/255.0f alpha:0];
        PAImageView *avaterImageView = [[PAImageView alloc]initWithFrame: CGRectMake(99, 221, 50, 50) backgroundProgressColor:faclor progressColor:[UIColor lightGrayColor]];
        [ myimageface setImageWithURL:[NSURL URLWithString:urlStr] placeholderImage:[UIImage imageNamed:@"viss_logo.png"] completed:^(UIImage *image, NSError *error, SDImageCacheType cacheType) {
            if (image!=nil) {
                  [avaterImageView updateWithImage:image animated:NO];
            }
          
            
            
        }];
        
        
        avaterImageView.frame=myimageface.frame;
        [avaterImageView updateWithImage:myimageface.image animated:NO];
        [self.view addSubview:avaterImageView];
        [myimageface setHidden:YES];
        
    }*/
}

-(void)showset
{
    /*if ([rightView isKindOfClass: [SecondDetailViewController class]]) {
        
        SecondDetailViewController *frmob=(SecondDetailViewController*)rightView;
        [frmob  clearImage];
        
        
        [nav.view removeFromSuperview];
        //        rightView=nil;
    }
    
    [TabHome.view setHidden:YES];
    [TabView1.view setHidden:YES];
    [TabView2.view setHidden:YES];
    [TabView3.view setHidden:YES];
    [TabView4.view setHidden:YES];
//    [LeGlobalObj sharedSingleton].SetPage=[[viewset alloc] init];
    [LeGlobalObj sharedSingleton].SetPage.view.frame=CGRectMake(148, 0, 320, 768);
    [self.view addSubview:[LeGlobalObj sharedSingleton].SetPage.view];*/
  
}

-(void)showPerson:(UIViewController *)vobj
{
//    [TabHome.view setHidden:YES];
//    [TabView1.view setHidden:YES];
//    [TabView2.view setHidden:YES];
//    [TabView3.view setHidden:YES];
//    [TabView4.view setHidden:YES];
    
    //[[LeGlobalObj sharedSingleton].SetPage.view removeFromSuperview];
    
    if (navP !=nil) {
        navP=nil;
    }
    navP= [[UINavigationController alloc] init];
    navP.view.frame=CGRectMake(148, 0, 320, 768);
    vobj.view.frame=CGRectMake(0, 0, 320, 768);
    leftView=navP.view;
 
    navP.navigationBarHidden=YES;
    [navP pushViewController:vobj animated:NO];
    [self.view addSubview:navP.view];
}

-(void)removeViewP
{
    [navP.view removeFromSuperview];
}
-(void)addchildview:(UIViewController *)conv
{

//    if (rightView!=nil) {
//        [rightView.view removeFromSuperview];
//        
//    }
    /*if ([rightView isKindOfClass: [SecondDetailViewController class]]) {
    
        SecondDetailViewController *frmob=(SecondDetailViewController*)rightView;
        [frmob  clearImage];
//        rightView=nil;
    }
    if (nav !=nil) {
        [nav.view removeFromSuperview];
        nav=nil;
    }
    nav=[[UINavigationController alloc] init];
    nav.view.frame=CGRectMake(468, 0, 556, 768);
    
    
    nav.navigationBarHidden=YES;
      [self.view addSubview:nav.view];
    [nav pushViewController:conv animated:YES];
    rightView=conv;*/
    //return;
 
}

-(void)removeView{
    
        [nav.view removeFromSuperview];
     
}
/*-(IBAction)click_home:(id)sender
{
#if DEBUG
    NSLog(@"Home clicked");
#endif
    //[[LeGlobalObj sharedSingleton].SetPage showlayer];
}
-(IBAction)click_notice:(id)sender
{
    
        //[[LeGlobalObj sharedSingleton].SetPage showlayer];
  //  [[LeGlobalObj sharedSingleton].SetPage showlayernotice:nil];
}

-(IBAction)click_home1:(id)sender
{
   // [self showHome];
    //[LeGlobalObj sharedSingleton].filter_str=@" and a.fromplat=0  ";
      // [TabHome loadnewormoredata];
}

-(IBAction)click_home2:(id)sender
{
   //     [self showHome];
  //   [LeGlobalObj sharedSingleton].filter_str=@"  and a.fromplat=1";
  //  TabHome.refreshing=YES;
  //  [TabHome loadnewormoredata];
}*/
-(void)refresh
{
   //    TabHome.refreshing=YES;
  //  [TabHome loadnewormoredata];
}
@end
