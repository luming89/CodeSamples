#python 2
#execfile("D:\\Web Scraping\\scrapingModule.py")

import urllib
import urllib2
#import urllib.request
import re

import sys  
from PyQt4.QtGui import *  
from PyQt4.QtCore import *  
from PyQt4.QtWebKit import *  


# debug switch
DEBUG = True;#True;
def retrieveProductInfo(htmltext):
  # regex for thumb and HD needs to be refined
  regex = [#('Product','<h2 class="productName">(.+?)</h2>'), # ARMANI
           #('Product','<span class="productName">(.+?)</span>'), # GAP
           ('Product','<title>(.+?)</title>'), # GAP
           ('Price','<span class="priceValue"><!---->(.+?)</span>'),
           ('Composition','<li>Composition:(.+?)</li>'),
           #('ItemCode','<span class="MFC">(.+?)</span>'), # ARMANI
           ('ItemCode','productPage.loadProductData\("(.+?)","initial","1"\)'), # GAP
           #('Thumb','<img style="width:100%" class="thumb&#xA;\t\t\t\t\tselected" src=(.+?)>'), #ARMANI
           ('Thumb','id="product_image"\n\t          src=\'(.+?)\''),
           ('HD','<img id="zoomImage" width="1920" height="2880" src=(.+?)>')]

  result = {};

  for Key,Value in regex:
      if DEBUG:
         print(Key+": "+Value)
      pattern = re.compile(Value, re.MULTILINE|re.DOTALL)
      result[Key]=re.findall(pattern,htmltext)
      if DEBUG:
         print(result[Key])
  
  # fetch images, a thumb and a HD
  if len(result['Product']):
  #if len(result['ItemCode']):
    if DEBUG:
       print("Found product: "+result['Product'][0])
    #if len(result['Thumb']):
       #urllib.urlretrieve(result['Thumb'][0].split('"')[1], result['ItemCode'][0]+"_thumb.jpg")
    #if len(result['HD']):
       #urllib.urlretrieve(result['HD'][0].split('"')[1], result['ItemCode'][0]+"_HD.jpg")
    
    # write results to a file
    try:
        outfile = open(result['ItemCode'][0]+".txt","w")
        for Key,Value in regex:
            outfile.write(Key+": ")
            for it in result[Key]:
                outfile.write(it+" ")
                outfile.write("\n")
        outfile.close()
        if DEBUG:
           print("File write completed")
    except IOError, e:
        print("File open failed")


class Render(QWebPage):  
  def __init__(self, url):  
    self.app = QApplication(sys.argv)  
    QWebPage.__init__(self)  
    self.loadFinished.connect(self._loadFinished)  
    self.mainFrame().load(QUrl(url))  
    self.app.exec_()  
  
  def _loadFinished(self, result):  
    self.frame = self.mainFrame()  
    self.app.quit()  
  
#url = "http://www.armani.com/us/emporioarmani/short-sleeve-t-shirt_cod37599060bq.html" # ARMANI
url = "http://www.gap.com/browse/product.do?cid=1026525&vid=1&pid=227345002" # GAP

# request = urllib2.Request(url)
# request.add_header('User-Agent', 'Mozilla/5.0')
# opener = urllib2.build_opener()
# htmltext = opener.open(request).read()

r = Render(url)  
htmltext = r.frame.toHtml()

if DEBUG:
    
    htmlfile = open("htmlfile_scraping_module.txt","w")
    htmlfile.write(htmltext)
    htmlfile.close()
    #print(htmltext)
retrieveProductInfo(htmltext)