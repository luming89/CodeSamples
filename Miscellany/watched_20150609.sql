-- phpMyAdmin SQL Dump
-- version 4.2.11
-- http://www.phpmyadmin.net
--
-- Host: 127.0.0.1
-- Generation Time: 2015-06-10 06:09:58
-- 服务器版本： 5.6.21
-- PHP Version: 5.6.3

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `movies`
--

-- --------------------------------------------------------

--
-- 表的结构 `watched`
--

CREATE TABLE IF NOT EXISTS `watched` (
  `Date Watched` date NOT NULL,
  `Name` varchar(255) NOT NULL DEFAULT 'UNDEFINED',
  `Year` smallint(6) NOT NULL,
  `Rating` tinyint(4) NOT NULL,
  `Director` varchar(255) NOT NULL,
  `Stars` varchar(255) NOT NULL,
  `Review` text NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `watched`
--

INSERT INTO `watched` (`Date Watched`, `Name`, `Year`, `Rating`, `Director`, `Stars`, `Review`) VALUES
('2011-08-28', 'Transformers: Dark of the Moon', 2011, 8, 'Michael Bay', 'Shia LaBeouf, Rosie Huntington-Whiteley, Tyrese Gibson', '商业大片，视觉冲击！'),
('2013-04-14', '八百壮士', 1975, 1, '丁善玺', '陈鸿烈，柯俊雄，林青霞，张艾嘉', '满分十分，我给一分。'),
('2013-04-15', '大武生', 2011, 7, '高晓松', '吴尊，韩庚，徐熙媛，刘谦', '就是看看高晓松导演的水平，还行吧。'),
('2013-05-13', 'Wreck-It Ralph', 2012, 9, 'Rich Moore', 'John C. Reilly, Jack McBrayer, Jane Lynch', '如果Video Games伴你走过了童年，如果你喜欢Disney的治愈系创意动画，不要错过它，Wreck-It Ralph. IMDb 7.9。'),
('2013-05-14', 'Tropa de Elite', 2007, 8, 'José Padilha', 'Wagner Moura, André Ramiro, Caio Junqueira', '不错，也更多地了解了里约。'),
('2013-05-15', '杀生', 2012, 8, '管虎', '黄渤，任达华，余男，苏有朋', '表面善良的村民愚昧地害死了自己的同胞。'),
('2013-05-16', 'Ben-Hur', 1959, 9, 'William Wyler', 'Charlton Heston, Jack Hawkins, Stephen Boyd', 'Classical and perfect!'),
('2013-05-17', 'The Graduate', 1967, 7, 'Mike Nichols', 'Dustin Hoffman, Anne Bancroft, Katharine Ross', '对剧情不能苟同，一个大学毕业生既然上了Mrs. Robinson还好意思喜欢并追求人家女儿？即使真心喜欢人家女儿理性也应该告诉他不能再追求了，必然要出大乱子，况且如果他是真心喜欢人家女儿为什么还要上Mrs. Robinson？'),
('2013-05-18', 'Casablanca', 1942, 9, 'Michael Curtiz', 'Humphrey Bogart, Ingrid Bergman, Paul Henreid', '为了伟大的反法西斯事业而放弃自己心爱的女人是需要多么大的勇气，经典！'),
('2013-05-19', 'Valter brani Sarajevo', 1972, 7, 'Hajrudin Krvavac', 'Velimir ''Bata'' Zivojinovic, Rade Markovic, Ljubisa Samardzic', '小时候的经典电影，重温'),
('2013-05-20', 'Taxi Driver', 1976, 9, 'Martin Scorsese', 'Robert De Niro, Jodie Foster, Cybill Shepherd', '影片所蕴涵的现实意义和深刻思想足以被列为世界百年电影中的极品，好片！'),
('2013-05-22', '海洋天堂', 2010, 7, '薛晓路', '李连杰，文章，桂纶镁，朱媛媛', '平淡中夹杂些许忧伤'),
('2013-05-27', 'The Last Emperor', 1987, 9, 'Bernardo Bertolucci', 'John Lone, Joan Chen, Peter O''Toole', '一群老外能拍出如此深刻反映中国一段历史的电影真是厉害！最后一个镜头溥仪回到紫禁城看着自己的龙椅，那应当是何种感受...凄凉？悲叹？作为最后一个皇帝溥仪真是不容易，看似是无限的自由，实现却被牢牢禁锢着...'),
('2013-05-28', 'Pulp Fiction', 1994, 9, 'Quentin Tarantino', 'John Travolta, Uma Thurman, Samuel L. Jackson', '黑色幽默，虽然理解得不深，但总感觉确实是部佳作'),
('2013-05-29', 'Amadeus', 1984, 8, 'Milos Forman', 'F. Murray Abraham, Tom Hulce, Elizabeth Berridge', '天才总有一些与众不同的特质，有的狂傲，有的偏执，影片中的Mozart属于前者。我很好奇有关Salieri的故事是真的吗？'),
('2013-05-30', 'Cleopatra', 1963, 8, 'Joseph L. Mankiewicz', 'Elizabeth Taylor, Richard Burton, Rex Harrison', '缓慢的节奏并不影响表达艳后的雄心壮志，可惜Antony只是一介武夫，无法像Julius Caesar那样帮助艳后实现梦想。PS：我本以为我会坚持不了四个多小时的电影，结果...果然大手笔，绝对是那个时代最顶级的布景、化妆。'),
('2013-06-01', 'Roman Holiday', 1953, 9, 'William Wyler', 'Gregory Peck, Audrey Hepburn, Eddie Albert', '小时候就听说了这部电影，直接今天才看，不愧是经典。影片反映了生活中的两个方面，自由与爱情。看似上流的贵族生活其实是多么无趣，公主只有脱离了这层枷锁才能去寻找自己的世界。在快节奏的今天，何尝不是这样呢？'),
('2013-06-02', '127 Hours', 2010, 8, 'Danny Boyle', 'James Franco, Amber Tamblyn, Kate Mara', '故事告诉我们：买劣质工具组最终坑的是自己...晕血者不宜观看本片。'),
('2013-06-03', '百变星君', 1995, 7, '叶伟民', '周星驰，梁咏琪，吴孟达，郑祖', '听说跳舞那段是抄Pulp Fiction的，我就是验证一下，果然是～'),
('2013-06-04', 'Contraband', 2012, 7, 'Baltasar Kormákur', 'Mark Wahlberg, Giovanni Ribisi, Kate Beckinsale', '还行，有点看头儿'),
('2013-06-05', 'The Next Three Days', 2010, 7, 'Paul Haggis', 'Russell Crowe, Elizabeth Banks, Liam Neeson', '女主角既然是冤枉的，怎么一点冤枉的样子都没有？还有警察最后是什么意思，YY出来女主角是清白的？'),
('2013-06-06', 'The Three Musketeers', 2011, 10, 'Paul W.S. Anderson', 'Logan Lerman, Matthew Macfadyen, Ray Stevenson', '宏大的场景，移轴的效果，瓦叔、Alice的加盟...生化危机导演拍的火枪手果然不一样，很合我口味。虽然评分不高，但我给满分！'),
('2013-06-07', 'The Princess Bride', 1987, 8, 'Rob Reiner', 'Cary Elwes, Mandy Patinkin, Robin Wright', '一部讲童话的影片，爱情与冒险，还可以～'),
('2013-06-08', 'Django Unchained', 2012, 9, 'Quentin Tarantino', 'Jamie Foxx, Christoph Waltz, Leonardo DiCaprio', 'Quentin和Waltz真是一对绝配！有瓦叔的电影就是好看！'),
('2013-06-09', 'Django', 1966, 7, 'Sergio Corbucci', 'Franco Nero, José Canalejas, José Bódalo', '因Django Unchained而来，不过没觉得有多经典...'),
('2013-06-10', 'Troy', 2004, 10, 'Wolfgang Petersen', 'Brad Pitt, Eric Bana, Orlando Bloom', '重温史诗级的经典，三个小时转眼即逝。来自伊利亚特的故事讲述了幼稚的帕里斯遇到美貌的海伦，注定要葬送坚不可摧的特洛伊城而成就阿基里斯不朽的荣耀。'),
('2013-06-11', 'Dirty Girl', 2010, 8, 'Abe Sylvia', 'Juno Temple, Jeremy Dozier, Milla Jovovich', '因面粉教育法慕名而来，挺好看，不过觉得Danielle的生父真不是东西，一点人性都没有...Juno Temple女神哇～'),
('2013-06-12', 'Gone with the Wind', 1939, 8, 'Victor Fleming', 'Clark Gable, Vivien Leigh, Thomas Mitchell', '终于看完了，觉得好长...应该是因为我并不喜欢Scarlett这个角色吧，一个已经成家好几次了的女人还非要想着自己的初恋Ashley，非要打破这个家庭的和谐，从情商角度来看她并非一位贤妻良母。'),
('2013-06-13', '铜雀台', 2012, 6, '赵林山', '周润发，刘亦菲，玉木宏，苏有朋', '因为三国杀而来，虽说汉献帝是逊帝，那也不能演得跟个屌丝一样，一点帝王气都没有...台词设计得也是一塌糊涂...因为神仙姐姐在，给个六分吧...'),
('2013-06-14', 'Some Like It Hot', 1959, 9, 'Billy Wilder', 'Marilyn Monroe, Tony Curtis, Jack Lemmon', '不愧是经典喜剧！毫不庸俗并且欢乐不断，笑点层出，再加上女神梦露的完美表现，本片不容错过～'),
('2013-06-15', 'Nanking', 2007, 8, 'Bill Guttentag, Dan Sturman', 'Hugo Armstrong, Rosalind Chao, Stephen Dorff', '前事不忘后事之师，永远不要放下武器。美国宪法修正案第二条精神：保证人民持枪的权利，我们这个民族就永远不会被奴役、屠杀。'),
('2013-06-16', '梅兰芳', 2008, 6, '陈凯歌', '黎明，章子怡，余少群，陈红', '为毛不让余少群一直演？换成黎明后就看不下去了，平淡无奇。还有讲日语时为毛不做官方字幕？不知道日语一般只是第二外语么？'),
('2013-06-17', 'Asterix et Obelix: Mission Cleopatra ', 2002, 8, 'Alain Chabat', 'Gérard Depardieu, Christian Clavier, Jamel Debbouze, Édouard Baer, Monica Bellucci', '挺不错的喜剧，既猛又萌的高卢人，美丽的艳后及侍女们，再加上当陪衬的罗马人，上演了一出无厘头的喜剧故事～画面精美艳丽，大爱～'),
('2013-06-18', '白鹿原', 2012, 8, '王全安', '张丰毅，张雨绮，段奕宏，吴刚', '套用里根总统的“不是枪杀人，是人杀人”这句话来总结一下观后感，“不是人杀人，是愚昧杀人”。'),
('2013-06-19', 'Waterloo Bridge', 1940, 8, 'Mervyn LeRoy', 'Vivien Leigh, Robert Taylor, Lucile Watson', '多美好的爱情，却因有了一点点迫不得已的瑕疵而终结...好伤感，愿完美主义者能看开些，活得轻松些...'),
('2013-06-20', '低俗喜剧', 2012, 9, '彭浩翔', '杜汶泽，郑中基，陈静，邵音音', '感觉挺实在的，可惜大陆没有影片分级制度，影响了不少电影...总体不错，挺好看挺好玩～'),
('2013-06-23', 'Born on July 4th', 1989, 8, 'Oliver Stone', 'Tom Cruise, Raymond J. Barry, Caroline Kava', '沉重的电影，讲述了美国的伤痛——越战。越战因为不为人知的私利而牺牲了一代人。为了报效祖国走上越战前线的陆战队队员搭上了自己的双腿，回到家乡后却不被同胞所理解，精神几近被摧毁。最后他终于明白报效祖国不是那么简单的一件事，并说出了战争的真相...'),
('2013-06-24', '画皮', 2008, 7, '陈嘉上，钱永强', '赵薇，周迅，陈坤，甄子丹', '来自聊斋的故事，顶一个～'),
('2013-06-25', '画皮II', 2012, 9, '乌尔善', '赵薇，周迅，陈坤，杨幂', '不错，比画皮一好多了，起码没有小唯脱掉人皮后的坑爹设计了。画面有扎克的范儿，像精美的油画一般，赞一个！'),
('2013-06-26', 'Pretty Woman', 1990, 9, 'Garry Marshall', 'Richard Gere, Julia Roberts, Jason Alexander', '很温馨的电影，现代灰姑娘的故事。女主角麻雀变凤凰，男主角变得更温情，最终没有选择收购船厂，而是帮助船厂运营下去。'),
('2013-06-27', 'Saving Private Ryan', 1998, 10, 'Steven Spielberg', 'Tom Hanks, Matt Damon, Tom Sizemore', '战争电影中的里程碑，逼真地反映了战争的残酷、人性的光辉。八名突击队队员为了救回四兄弟里仅存的James Ryan而深入敌后，找到了他并帮助他保卫了阵地，不过队长及大部分队员为国捐躯，成为美国的荣耀。'),
('2013-06-29', '恋爱通告', 2010, 9, '王力宏', '王力宏，刘亦菲，陈汉典，曾轶可', '高富帅微服私访记...有亦菲的电影总有这种感觉，仿佛亦菲是来自另一个纯洁的世界，其余人都在此俗世...'),
('2013-07-15', '致我们终将逝去的青春', 2013, 7, '赵薇', '杨子姗，赵又廷，韩庚，江疏影', '电影水平一般，剧情狗血，但打的青春牌让无数人想起自己青葱的岁月，怀念自己逝去的青春...'),
('2013-07-18', '300', 2006, 10, 'Zack Snyder', 'Gerard Butler, Lena Headey, David Wenham', '油画般的画面，史诗级的电影...特效控扎克总是能带来完美的视觉冲击...This is Sparta. We fight as a single, impenetrable phalanx. We do what we are trained to do, what we are bred to do and what we are born to do.'),
('2013-07-24', '建国大业', 2009, 9, '韩三平，黄建新', '唐国强，张国立，许晴，刘劲', '在看了一季晓说后，再看这部三年前没怎么看懂的电影就好多了，电影中蒋中正的一句话令人印象深刻：反（腐）则亡党，不反则亡国。'),
('2013-07-25', 'The Water Horse', 2007, 8, 'Jay Russell', 'Emily Watson, David Morrissey, Alex Etel', '水怪的存在与否其实已经无所谓了，重要的是它作为一种标志、象征，每年为风景如画的苏格兰吸引成千上万的游客。大爱电影中苏格兰式的幽默！'),
('2013-07-26', '大兵小将', 2010, 7, '丁晟', '成龙，王力宏', '一般，有一点喜剧元素，多亏了王宝强。在安分守己的老兵和激昂骁勇的将军的回梁国路上讲了一些哲理。'),
('2013-07-28', 'Monty Python and the Holy Grail', 1975, 6, 'Terry Gilliam, Terry Jones', 'Graham Chapman, John Cleese, Eric Idle', '虽说是英式无厘头幽默，但也没觉得多么出类拔萃啊...莫非因为是无厘头的先驱者？不过片头那个玩泥的农民抨击君主体制和阶级压迫相当有意思。'),
('2013-07-31', 'Wizard of Oz', 1939, 7, 'Victor Fleming', 'Judy Garland, Frank Morgan, Ray Bolger', '看完发现自己已经老了，除了音乐其它的已经欣赏不了了...年轻真好...超龄观众很不好意思地给个保守分...'),
('2013-08-05', 'Up', 2009, 8, 'Pete Docter, Bob Peterson', 'Edward Asner, Jordan Nagai, John Ratzenberger', '老爷爷和老奶奶相濡以沫的一生真是令人动容，最终老爷爷一个人完成了夫妇二人毕生的梦想——安家于天堂瀑布。'),
('2013-08-14', '厨子戏子痞子', 2013, 9, '管虎', '刘烨，张涵予，黄渤', '有创意，画图艳丽，有点像扎克的Sucker Punch，剧情也赞，不知道是不是真的，好电影！'),
('2013-08-15', '北京遇上西雅图', 2013, 8, '薛晓路', '汤唯，吴秀波', '很准确地刻画了异国小三儿寂寞的心理，女强人奋斗的生活和一个温暖大叔的故事，不错。'),
('2013-08-16', 'Death of a Salesman', 1985, 8, 'Volker Schlöndorff', 'Dustin Hoffman, Kate Reid, John Malkovich', '一个天真、愚昧、虚荣的推销员教育了两个花花公子、无业游民的儿子，只有一位母亲保持着清醒和理性，可惜已经无济于事，还得生活在痛苦中。这样的悲剧虽然残酷，但也终究是由上演悲剧的人造成的。可怜之人必有可恨之处。一部深刻的电影。'),
('2013-08-21', 'The Shawshank Redemption', 1994, 10, 'Frank Darabont', 'Tim Robbins, Morgan Freeman, Bob Gunton', '不愧是顶级电影，看似平淡的剧情却蕴含了无限的人生感悟，如希望、友情等。其实奇迹无不是多么的罕见，只要保持心中的希望，为了目标努力地去奋斗，无论环境有多么的恶劣，终有一天会像安迪一样完成自己的奇迹，重归美好的生活。太多的人因为放弃希望而与自己的梦想擦肩而过，从此抱憾终生。'),
('2013-08-23', 'Nuovo cinema Paradiso ', 1988, 7, 'Giuseppe Tornatore', 'Philippe Noiret, Enzo Cannavale, Antonella Attili', '节奏缓慢的电影，一个小镇的影院见证了一个男孩的成长，一个小镇的兴衰，一个时代的变迁，最终划入史册。“人生和电影不一样，人生比电影苦多了”。'),
('2013-09-02', 'District 9', 2009, 8, 'Neill Blomkamp', 'Sharlto Copley, David James, Jason Cope', '为了利益而出卖同胞的生命，人性的丑恶面也会另人毛骨悚然。画面掌控得刚刚好，本以为会很血腥、恶心，但实际上还是可以接受的。'),
('2013-10-19', 'The Debt', 2010, 7, 'John Madden', 'Helen Mirren, Sam Worthington, Tom Wilkinson', '感觉没什么特别出彩的...一般般吧'),
('2014-05-27', 'Despicable Me 2', 2013, 9, 'Pierre Coffin, Chris Renaud', 'Steve Carell, Kristen Wiig, Benjamin Bratt', 'Love it so much! Especially those funny minions!'),
('2014-05-29', 'Pacific Rim', 2013, 10, 'Guillermo del Toro', 'Idris Elba, Charlie Hunnam, Rinko Kikuchi', 'Love the most! Wonderful visual effects! Quote: The world is coming to an end. So where would you rather die? Here? or in a Jaeger!'),
('2014-06-01', 'The Wolf of Wall Street', 2013, 10, 'Martin Scorsese', 'Leonardo DiCaprio, Jonah Hill, Margot Robbie', 'The coolest movie I have ever seen! "If the U.S. Justice Department sent us a subpoena, it would become papier toilette. We would wipe our ass with it."'),
('2014-08-20', 'Seediq Bale (Part A)', 2011, 9, 'Te-Sheng Wei', 'Masanobu Andô, Jun''ichi Haruta, Sabu Kawahara', 'Awesome. A Taiwanese version of Brave Heart. Forever free. I think Japanese should be clever enough to rule Taiwan(as they showed their weapons to the natives and so on), but they treat the natives unwisely.'),
('2014-08-21', 'Seediq Bale (Part B)', 2011, 9, 'Te-Sheng Wei', 'Masanobu Andô, Jun''ichi Haruta, Sabu Kawahara', 'An extremely vivid movie that you can feel everything just as you were there. Although the natives are ignorant enough to chanllenge each other and caused wars among themselves, they fought bravely against the Japanese and reminded the Japanese of their Bushido spirit.'),
('2014-08-23', 'The General''s Daughter', 1999, 8, 'Simon West', 'John Travolta, Madeleine Stowe, James Cromwell', 'Investigator Brenner is a genius with gut. However, the general Campbell is THE son of bitch. He even traded her daughter''s dignity for a higher rank, which made her daughter desperate & plotted the whole story.'),
('2014-10-17', 'American Hustle', 2013, 8, 'David O. Russell', 'Christian Bale, Amy Adams, Bradley Cooper', 'Actually didn''t get the whole story from it... A little too complicated for me... As a foreigner in the States. However it''s still very cool for me anyways!'),
('2014-12-23', 'Now You See Me', 2013, 10, 'Louis Leterrier', 'Jesse Eisenberg, Common, Mark Ruffalo', 'An awesome movie with the most creative thoughts come from the principles of magic! Nice plot, nice effects and attractive actresses. Definitely one of my favourites!'),
('2014-12-29', 'Captain Phillips ', 2013, 9, 'Paul Greengrass', 'Tom Hanks, Barkhad Abdi, Barkhad Abdirahman', 'True story is the ultimate source of charm. Not bad!'),
('2015-01-02', 'The Hobbit: An Unexpected Journey', 2012, 10, 'Peter Jackson', 'Martin Freeman, Ian McKellen, Richard Armitage', '"True courage is about knowing not when to take a life, but when to spare one." - Gandalf'),
('2015-01-11', 'Olympus Has Fallen', 2013, 7, 'Antoine Fuqua', 'Gerard Butler, Aaron Eckhart, Morgan Freeman', 'We''ve lost good friends. Family. All good people. Heroes, every one of them. Our hearts and prayers go out to their families. And they will be remembered. Nor will we forget those who serve out of the spotlight, to whom we owe our highest gratitude. Our foe did not come only to destroy our things or our people. They came to desecrate our way of life. To foul our beliefs. Trample our freedom. And in this, not only did they fail, they granted us the greatest gift - a chance at our rebirth. We will rise renewed, stronger, and united. This is our time. Our chance to get back to the best of who we are. To lead by example with the dignity, integrity, and honor that built this country. And which will build it once again. May God bless you, and may God bless the United States of America.'),
('2015-01-24', '智取威虎山', 2014, 9, '徐克', '张涵予，梁家辉，林更新，余男，佟丽娅', 'This movie reminds me of 300 by Zach Schneider, although they are not in the same level. It is well-made and creative. From this I just feel the hope of Chinese movies.'),
('2015-02-21', 'Escape Plan', 2013, 8, 'Mikael Håfström', 'Sylvester Stallone, Arnold Schwarzenegger', 'Cool! A pretty awesome movie with both Stallone and Schwarzenegger.'),
('2015-02-28', '甲午风云', 1962, 8, '林农', '李默然', 'I watched this movie for the first time when I was a kid. I felt so annoyed as a Chinese in the period. Now I have grown up and I watched it again. This time I feel not only annoyed, but also stupid as a Chinese. We fool our people so long and most of the governors are cowards and selfish. More than one hundred years have passed, I don''t think there are significant changes. Just feel no hope for our nation...'),
('2015-03-09', 'Sleepless in Seattle', 1993, 9, 'Nora Ephron', 'Tom Hanks, Meg Ryan, Ross Malinger', 'Got an offer from Amazon, Seattle today so I rushed to watch this romantic movie. Quite warm and I like it. "Like do you believe in heaven?""I never did. ... I have these dreams about... your mom..."'),
('2015-03-13', '中国合伙人', 2013, 7, '陈可辛', '黄晓明，邓超，佟大为', '“梦想是什么，梦想是你坚持时就会感到幸福的东西。”剧情略凌乱，但主旨深得我心：人生在世应该有自己的梦想并为之奋斗，在将来白发苍苍时不会为自己虚度了的青春而弄到惋惜。最后还是要向这些追求梦想坚持不懈的楷模们致敬：柳传志，马云，杨澜，俞敏洪，徐小平，王强，李开复，张朝阳等。'),
('2015-03-21', 'Predator', 1987, 9, 'John McTiernan', 'Arnold Schwarzenegger', 'Rewatched this classic movie this afternoon because I''m cherishing the memory of childhood. Although the technology is out-of-date at this time, the movie still attracted me firmly. Nice plot & Schwarzenegger!'),
('2015-03-22', 'Predator 2', 1990, 7, 'Stephen Hopkins', 'Danny Glover', 'Watched this because of Predator. However, it is that not as attractive as Predator, maybe due to lack of Schwarzenegger. Anyway, I enjoyed Predator series.\r\n'),
('2015-03-31', 'The Theory of Everything', 2014, 9, 'James Marsh', 'Eddie Redmayne, Felicity Jones, Tom Prior', 'The performance of the actor who cast Hawking impressed me extraodinarily... The movie depicts the life of Hawking pretty well. I enjoyed this one!'),
('2015-04-01', 'Big Hero 6', 2014, 9, 'Don Hall, Chris Williams', 'Ryan Potter, Scott Adsit, Jamie Chung', 'Disney''s story is always so warm. Hi, I''m Baymax! Your personal healthcare companion. Hope you are satisfied with your care! PS: I saw that CS559 alumnus in the credits~ He developed the snow effect for Disney!'),
('2015-04-05', 'Face/Off', 1997, 9, 'John Woo', 'John Travolta, Nicolas Cage, Joan Allen', 'This movie was mentioned in Morning Call as the director is John Woo so I rushed to watch it. Besides the great effects and plot, I also found a surprise! That famous scenario in 3DMark comes from this movie!'),
('2015-04-07', '地道战', 1965, 9, '任旭东', '朱龙广，王炳彧，张勇手，刘秀杰', 'Childhood classic. It''s kinda funny to watch this at Madison. Anyway, I enjoyed it! BTW, there is something worth mentioning. "One hundred Japs, two hundred traitors", while Yamata didn''t surrender at all.'),
('2015-04-12', 'Güeros', 2014, 5, 'Alonso Ruiz Palacios', 'Tenoch Huerta, Sebastián Aguirre, Ilse Salas', 'Although it is classified as comedy, I believe it is more close to artistic. However, it is indeed funny sometimes, but I don''t know where the fun comes from. The director seems to be good at using sound effects, even with high frequency noise, which is really annoying. It is a little exotic and reminds me of my childhood, maybe due to the black-and-white property and the old-time scenario. All in all, I basically cannot understand this movie, and it quite like the artistic version of Hidden Track in my opinion.'),
('2015-04-19', '地雷战', 1962, 8, '唐英奇，徐达', '白大钧，张长瑞，吴健海，张杰', 'Childhood classic too. It is heavily imbalanced... But no surprise on this. I regard this one as a dual of "地道战", and enjoyed them a lot when I was a child!'),
('2015-04-21', 'White House Down', 2013, 7, 'Roland Emmerich', 'Channing Tatum, Jamie Foxx, Maggie Gyllenhaal', 'Use this as a time killer. A standard commericial movie. You can guess the end when you just saw the beginning.'),
('2015-04-23', '小兵张嘎', 1963, 8, '崔嵬，欧阳红樱', '安吉斯，李健吾，于中义，周森冠', 'Another childhood classic! Also imbalanced... So many adults were beated by a kid... I regard this one as a dual of "鸡毛信", and enjoyed them a lot when I was a child!'),
('2015-04-25', 'The Hunger Games', 2012, 8, 'Gary Ross', 'Jennifer Lawrence, Josh Hutcherson, Liam Hemsworth', 'I watched this when I was in The Great Smoky. Today I watched this again with Yinghan and Yutong as the finale for this day. Yutong believes that the 13 district represents the first 13 states of the US. What a sharp point! It makes a lot of sense! By the way, what a nice day, and Mazda!'),
('2015-04-28', '鸡毛信', 1954, 8, '石挥', '舒适，蔡元元，李保罗，曹铎', 'Childhood classic series. This one feels like the dual of 小兵张噶. And have all the common characteristics as the other anti-Japs childhood movies.'),
('2015-05-05', 'The Hunger Games: Catching Fire', 2013, 8, 'Francis Lawrence', 'Jennifer Lawrence, Josh Hutcherson, Liam Hemsworth', 'Just a standard commercial movie, a sequel of The Hunger Games, and used as a time-killer.'),
('2015-05-07', '三毛从军记', 1992, 9, '张建亚，崔杰', '贾林，魏宗万，孙飞虎，朱艺', 'Childhood classic. It indeed exceeds my expectation, although it''s not the first time I watch it. The high-caliber sarcasm is so impressive and talented. And the comedy part reminds me of Home Alone. Perfect!'),
('2015-05-17', 'The Interview', 2014, 9, 'Evan Goldberg, Seth Rogen', 'James Franco, Seth Rogen, Randall Park', 'This movie is fucking amazing and funny! Enjoyed so much! Ye, I think it is irony enough and Kim Family will definitely hate it a lot, but I believe it''s telling the truth to some extent. '),
('2015-06-05', 'Batman Begins', 2005, 8, 'Christopher Nolan', 'Christian Bale, Michael Caine, Ken Watanabe', 'Nolan brings humanity into this commercial movie and makes it much better! "Training is nothing, will is everything!"'),
('2015-06-08', 'The Dark Knight', 2008, 9, 'Christopher Nolan', 'Christian Bale, Heath Ledger, Aaron Eckhart', 'Wonderful plot! Which catches me firmly! I become a fan of Nolan because of this movie! By the way, the equipments are also so cool!');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `watched`
--
ALTER TABLE `watched`
 ADD PRIMARY KEY (`Date Watched`), ADD KEY `Name` (`Name`);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
