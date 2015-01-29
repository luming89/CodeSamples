//
//  FirstPageViewController.m
//
//  Created by Luming on 12-21-2014.
//  Copyright (c) 2014 Luming. All rights reserved.
//
#import "FirstPageViewController.h"
#import "APService.h"
#import "AppDelegate.h"
#import "RegisterViewController.h"
#import "LoginViewController.h"
#import "HomeViewController.h"
#import "FindPwViewControll.h"
#import "LeGlobalObj.h"
#import "YMAPIReader3.h"
#import "Config.h"
#import "Macros.h"

#define FIRST_PAGE_SCROLL_TIME_INTERVAL 4 // time interval between image switch

CGFloat alpha=1.0;
CGFloat alphab=0.0;
CGFloat clickalpha=1.0;

NSTimer *timer;         // Timer to make auto-scrolling in first page
NSTimer *clicktimer;    // Timer to give the animation when click on an option
BOOL isFirst=YES;
BOOL isClick=NO;
int i=0;
int j=0;   

int whichPage=1;       // Indicator of the current page in auto scrolling 

@interface FirstPageViewController () <UIScrollViewDelegate>

-(void)function;

@property (nonatomic, strong) IBOutlet UIView *view; 
@property (nonatomic, strong) IBOutlet UIView *showView;
@property (nonatomic, strong) IBOutlet UIImageView *firstPage;
@property (nonatomic, strong) IBOutlet UIScrollView *scrollView;
@property (nonatomic, strong) IBOutlet UIButton *signUp;
@property (nonatomic, strong) IBOutlet UIButton *login;
@property (nonatomic, strong) IBOutlet UIButton *takeALook;
@property (nonatomic, strong) IBOutlet UIPageControl *pageControl;
@property (nonatomic, strong) IBOutlet UIImageView  *firstPageLogo;
@property (nonatomic , strong) NSTimer *animationTimer;
@property (nonatomic , assign) NSTimeInterval animationDuration;


-(IBAction)take_a_look_action:(id)sender;
-(IBAction)signupaction:(id)sender;
-(IBAction)loginaction:(id)sender;

@end

@implementation FirstPageViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
        // Custom initialization
        tend=YES;
    
        [LeGlobalObj sharedSingleton].tokenStr= [APService registrationID];
        [self autologin];
        
    }

    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // Do any additional setup after loading the view from its nib.
    CGSize size=[[UIScreen mainScreen]bounds].size;
    CGFloat screenHeight=size.height;
    if (screenHeight>500.0)
    {
        [[NSBundle mainBundle] loadNibNamed:@"FirstPageViewController" owner:self options:nil];
    }else {
        [[NSBundle mainBundle] loadNibNamed:@"FirstPageViewController3.5" owner:self options:nil];
    }
    
    [self configView];
    [self changeAlphy];
    [self languageChoose];
    time=0;
}


-(void)languageChoose
{
    // To be generalized

    [self.signup setBackgroundImage:[UIImage imageNamed:@"viss_signupen.png"] forState:UIControlStateNormal];
    [self.signup setBackgroundImage:[UIImage imageNamed:@"viss_signup_highlighten.png"] forState:UIControlStateHighlighted];
    [self.login setBackgroundImage:[UIImage imageNamed:@"viss_loginen.png"] forState:UIControlStateNormal];
    [self.login setBackgroundImage:[UIImage imageNamed:@"viss_login_highlighten.png"] forState:UIControlStateHighlighted];
    [self.takealook setBackgroundImage:[UIImage imageNamed:@"viss_take_a_looken.png"] forState:UIControlStateNormal];
    [self.takealook setBackgroundImage:[UIImage imageNamed:@"viss_takealook_highlighten.png"] forState:UIControlStateHighlighted];

}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    
}

 
- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:YES];
    [self languageChoose];
    i=0;
    j=0;
    [timer setFireDate:[NSDate distantFuture]];
    [clicktimer setFireDate:[NSDate distantFuture]];
    
    self.signup.frame=frame1;
    self.login.frame=frame2;
    self.login.frame=frame3;
}

- (void)viewDidAppear:(BOOL)animated
{
CGSize size=[[UIScreen mainScreen]bounds].size;
    CGFloat screenHeight=size.height;
    if (screenHeight>500.0) 
    {
       self.signup.frame=CGRectMake(40, 428, 67, 67);
       self.takealook.frame=CGRectMake(213, 428, 67, 67);
       self.login.frame=CGRectMake(127, 428, 67, 67);
       //        self.signup.alpha=1.0;
       //        self.login.alpha=1.0;
       //        self.takealook.alpha=1.0;
    }
    else 
    {
       self.signup.frame=CGRectMake(40, 356, 67, 67);
       self.takealook.frame=CGRectMake(213, 356, 67, 67);
       self.login.frame=CGRectMake(127, 356, 67, 67);
       
    }
           
    
    self.signup.alpha=1.0;
    self.login.alpha=1.0;
    self.takealook.alpha=1.0;
    [self   languageChoose];
}


-(void)configView
{
    
    CGSize size=[[UIScreen mainScreen]bounds].size;
    CGFloat screenHeight=size.height;

    if (screenHeight>500.0) {
        self.signup.frame=CGRectMake(40, 428, 67, 67);
        self.takealook.frame=CGRectMake(213, 428, 67, 67);
        self.login.frame=CGRectMake(127, 428, 67, 67);
        //        self.signup.alpha=1.0;
        //        self.login.alpha=1.0;
        //        self.takealook.alpha=1.0;
    }
    else
    {
        self.signup.frame=CGRectMake(40, 356, 67, 67);
        self.takealook.frame=CGRectMake(213, 356, 67, 67);
        self.login.frame=CGRectMake(127, 356, 67, 67);
        
    }
    
    self.signup.alpha=1.0;
    self.login.alpha=1.0;
    self.takealook.alpha=1.0;
    
    if (screenHeight>500.0) {
        [self.scrollview setContentSize:CGSizeMake(960, 568)];
    }else{
        [self.scrollview setContentSize:CGSizeMake(960, 480)];
    }
    
    
    [self.scrollview setShowsHorizontalScrollIndicator:NO];
    [self.scrollview setDelegate:self];
    [self.pageControl addTarget:self action:@selector(daninout:) forControlEvents:UIControlEventValueChanged];
    // Activate the timer controlling auto-scrolling
    timer=[NSTimer scheduledTimerWithTimeInterval:FIRST_PAGE_SCROLL_TIME_INTERVAL target:self selector:@selector(autoScrolling) userInfo:nil repeats:YES];
    [timer setFireDate:[NSDate distantPast]];
}


-(void)changeAlpha // analyze this method and comment
{
    
    timer=[NSTimer scheduledTimerWithTimeInterval:2 target:self selector:@selector(function) userInfo:nil repeats:YES];
    [timer setFireDate:[NSDate distantPast]];
    
    [self begani];
}

-(void)function // analyze this method and comment, BTW, bad naming, consider renaming
{
//    alpha=alpha-0.0005;
//    alphab=alphab+0.0005;
//    self.firstpage.alpha=alpha;
//    self.firstpagelogo.alpha=alpha;
//    
//    //NSLog(@"第一个定时器有执行吗！");
////    self.firstpagelogo.transform=CGAffineTransformMakeTranslation(0, -0.0815);
//    self.signup.alpha=alphab;
//    self.login.alpha=alphab;
//    self.takealook.alpha=alphab;
//    self.forgetpassword.alpha=alphab;
//    self.firstpagewritelogo.alpha=alphab;
    
    j++;
#if DEBUG
    NSLog(@"j=%i in function",j);
#endif
    if (j%2==0)
    {
        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:1.5f];
        
        self.firstpagelogo.alpha=0.0f;
        self.firstpagewritelogo.alpha=1.0f;
        [UIView commitAnimations];
        
        isClick=YES;
        [timer setFireDate:[NSDate distantFuture]];
    }
}
- (BOOL)shouldAutoRotate
{
    return YES;
}
-(NSUInteger)supportedInterfaceOrientations

{
    return UIInterfaceOrientationMaskPortrait;
    
}
-(void)takeLookRespond // analyze these clickfunction method and comment
{
    // invoked from take_a_look_action
    i++;
#if DEBUG
    NSLog(@"i=%i in takeLookRespond",i);
#endif
//
//    //NSLog(@"点击后的定时器方法有执行吗！");
    
    if (i==2)
    {
        HomeViewController *hvc=[[HomeViewController alloc]init];
        //[self.navigationController pushViewController:hvc animated:YES];
        [self presentViewController:hvc animated:YES completion:^{}];// test only
        //[self.view removeFromSuperview];
           //[[LeGlobalObj sharedSingleton].HomePage showface];
    }
    else if(i>2)
    {
        //[clicktimer setFireDate:[NSDate distantFuture]];
    }

}



-(void)signupRespond
{
    
    i++;
#if DEBUG
    NSLog(@"i=%i in signupRespond",i);
#endif

    
    if (i==2)
    {
        RegisterViewController *rvc=[[RegisterViewController alloc] init];
        [self presentViewController:rvc animated:YES completion:^{}];

    }
    else if(i>2)
    {
    
 
            [clicktimer setFireDate:[NSDate distantFuture]];
    }
}


-(void)loginRespond
{
    
    i++;
#if DEBUG
    NSLog(@"i=%i in loginRespond",i);
#endif
    
    if (i==2)
    {
        LoginViewController *lvc=[[LoginViewController alloc]init];
        [lvc setfrom:@"first"];
        [self presentViewController:lvc animated:YES completion:^{}];
      
    }
    else if(i>2)
    {
        isClick=YES;
        [clicktimer setFireDate:[NSDate distantFuture]];
    }
}

- (void)scrollViewWillBeginDecelerating:(UIScrollView *)scrollView
{
    
}


- (void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView
{
    CGFloat pageWidth = scrollView.frame.size.width;
    int page = floor((scrollView.contentOffset.x - pageWidth / 2) / pageWidth) + 1;
    if(page == 1) 
    {
        return;
    } 
    else if (page == 0) 
    {
        [self pageMoveToRight];
    } 
    else 
    {
        [self pageMoveToLeft];
    }
    
    CGPoint p = CGPointZero;
    p.x = pageWidth;
    [scrollView setContentOffset:p animated:NO];
    time++;
    
    if (time>2) 
    {
        time=0;
    }
    self.pageControl.currentPage=time;
    
}

- (void)pageMoveToRight 
{
    UIView *tmp = pageSecond;
    pageSecond = pageFirst;
    pageFirst = pageThird;
    pageThird = tmp;
    
    [self setPageFrame];
}

- (void)pageMoveToLeft
{
    UIView *tmp = pageSecond;
    pageSecond = pageThird;
    pageThird = pageFirst;
    pageFirst = tmp;
    
    [self setPageFrame];
}

-(void)setPageFrame 
{
    pageFirst.frame = CGRectMake(_scrollview.frame.origin.x, _scrollview.frame.origin.y, _scrollview.frame.size.width, _scrollview.frame.size.height);
    pageSecond.frame = CGRectMake(_scrollview.frame.origin.x + _scrollview.frame.size.width, _scrollview.frame.origin.y, _scrollview.frame.size.width, _scrollview.frame.size.height);
    pageThird.frame = CGRectMake(_scrollview.frame.origin.x + _scrollview.frame.size.width * 2, _scrollview.frame.origin.y, _scrollview.frame.size.width, _scrollview.frame.size.height);
}

- (void)scrollViewDidEndScrollingAnimation:(UIScrollView *)scrollView
{
    
}


-(IBAction)takeALookAction:(id)sender
{
#if DEBUG
    NSLog(@"DEBUG::: Take a look action.");
    if(isClick)
        NSLog(@"isClick is true");
    else
        NSLog(@"isClick is false");
#endif
    if (isClick)
    {
        clicktimer=[NSTimer scheduledTimerWithTimeInterval:1.5f target:self selector:@selector(takeLookRespond) userInfo:nil repeats:YES];
        [clicktimer setFireDate:[NSDate distantPast]];
        
        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:1.5f];
        
        self.signup.alpha=0.0f;
        self.login.alpha=0.0f;
        [UIView commitAnimations];
        
        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:1.5f];
        CGSize size=[[UIScreen mainScreen]bounds].size;
        CGFloat screenHeight=size.height;
        if (screenHeight>500.0)
        {
            self.takealook.frame=CGRectMake(127, 428, 67, 67);
        }
        else
        {
            self.takealook.frame=CGRectMake(127, 356, 67, 67);
        }

     
        [UIView commitAnimations];
    }
}


-(IBAction)signupAction:(id)sender
{
    if (isClick) {
        clicktimer=[NSTimer scheduledTimerWithTimeInterval:1.5f target:self selector:@selector(signupRespond) userInfo:nil repeats:YES];
        [clicktimer setFireDate:[NSDate distantPast]];
        
        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:1.5f];
        
        self.login.alpha=0.0f;
        self.takealook.alpha=0.0f;
        [UIView commitAnimations];
        
        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:1.5f];
        
        CGSize size=[[UIScreen mainScreen]bounds].size;
        CGFloat screenHeight=size.height;
        if (screenHeight>500.0) 
        {
            self.signup.frame=CGRectMake(127, 428, 67, 67);
        }
        else
        {
            self.signup.frame=CGRectMake(127, 356, 67, 67);
        }
        [UIView commitAnimations];
    }
}



-(IBAction)loginAction:(id)sender
{
    if (isClick)
    {
        clicktimer=[NSTimer scheduledTimerWithTimeInterval:2.0f target:self selector:@selector(loginRespond) userInfo:nil repeats:YES];
        [clicktimer setFireDate:[NSDate distantPast]];
        
        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:1.5f];
        
        self.signup.alpha=0.0f;
        self.takealook.alpha=0.0f;
        [UIView commitAnimations];
        
        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:1.5f];
        
        CGSize size=[[UIScreen mainScreen]bounds].size;
        CGFloat screenHeight=size.height;

        if (screenHeight>500.0) 
        {
            self.signup.frame=CGRectMake(127, 428, 67, 67);
            self.takealook.frame=CGRectMake(127, 428, 67, 67);
        }
        else
        {
            self.signup.frame=CGRectMake(127, 356, 67, 67);
            self.takealook.frame=CGRectMake(127, 356, 67, 67);
        }
        [UIView commitAnimations];
    }
}

-(void)autoScrolling
{
    time++;
    CGFloat pageWidth = _scrollview.frame.size.width;
    int page = floor((_scrollview.contentOffset.x - pageWidth / 2) / pageWidth) + 1;
    
    if(page == 1) {
        return;
    } else if (page == 0) {
        [self pageMoveToRight];
    } else {
        [self pageMoveToLeft];
    }
    
    CGPoint p = CGPointZero;
    p.x = pageWidth;
    [_scrollview setContentOffset:p animated:NO];
    
     [self.scrollview setContentOffset:CGPointMake(self.scrollview.contentOffset.x+320, 0) animated:YES];
    if (time>2)
    {
        time=0;
    }
    self.pageControl.currentPage=time;
    whichPage=time+1;
}

-(IBAction)clickPrev:(id)sender
{
    FindPwViewControll *frmobj=[[FindPwViewControll alloc] init];
    [self presentViewController:frmobj animated:YES completion:^{
    }];
}

-(void)autoLogin
{
   NSString *filename1=[[LeGlobalObj sharedSingleton] GetFileName:@"username.txt"];
   if (![[LeGlobalObj sharedSingleton] is_file_exist:filename1]) {
       return;
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
           AppDelegate *appdelegate=[[UIApplication sharedApplication]delegate];
           appdelegate.loginuserid=dict[@"id"];
           appdelegate.loginusername=dict[@"name"];
           appdelegate.loginavatar=dict[@"avatar"];
           viewHome *second=[[viewHome alloc]init];
           [self.navigationController pushViewController:second animated:YES];
       }
       

   }];
}

@end
