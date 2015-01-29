#python2 execfile("D:\\Web Scraping\\crawler.py")
#exec(open("D:\\Web Scraping\\crawler.py").read())
import time



import urlparse
#import urllib.parse
import urllib
import urllib2
import re
import os

import sys  

# render the page
from PyQt4.QtGui import *  
from PyQt4.QtCore import *  
from PyQt4.QtWebKit import *  

#import urllib.request
from bs4 import BeautifulSoup

#
def retrieveProductInfo(htmltext):
  # regex for thumb and HD needs to be refined
  regex = [('title','<title>(.+?)</title>'),
          #('Product','<h2 class="productName">(.+?)</h2>'), # ARMANI
           ('Product','<div id="productNameText">\s*<h1>\s*(.+?)\s*</h1>\s*</div>'), # GAP
           #('Price','<span class="priceValue"><!---->(.+?)</span>'), # ARMANI
           ('Price','<br />\s*\w* - \$(.+?) <br />'),# GAP
           ('Composition','<li>Composition:\xc2\xa0\s*(.+?)</li>'),
           #('ItemCode','<span class="MFC">(.+?)</span>'), # ARMANI
           ('ItemCode','productPage.loadProductData\("(.+?)","initial","1"\)'),
           #('ItemCode','<div id="productNumber">#(.+?)<div id="productPageVendorId" style="display:none;"></div></div>'), # GAP
           #('Thumb','<img style="width:100%" class="thumb&#xA;\t\t\t\t\tselected" src="(.+?)" />'),
           ('Thumb','<img style="width:100%" class="thumb&#xA;\t\t\t\t\t" src="(.+?)" />'),
           ('HD','<img id="zoomImage" width="1920" height="2880" src="(.+?)" />'),
           ('Color','<a href="#" title=".+?" data-selection="Colors:.+?">(.+?)</a>'),  # why I have to add ? in title=".+?"
           ('Series','"socialDescription":"(.+?)"'),
           
           #('Gender','"gender":"(.+?)"'),
           ('Gender','commonChannelName = unUnicode\("(.+?)"\)') # GAP
           ]

  result = {};
  
  for Key,Value in regex:
      if DEBUG:
         print(Key+": "+Value)
      pattern = re.compile(Value, re.MULTILINE|re.DOTALL)
      result[Key]=re.findall(pattern,htmltext)
      size = len(result[Key])
      i = 0
      for i in range(0, size):
        result[Key][i] = re.sub(r'[\xc2\xa0]',' ',result[Key][i])
      if DEBUG==False:
         print(result[Key])

  

  
  # fetch images, a thumb and a HD
  if len(result['Product']):
  #if len(result['ItemCode']):
    if DEBUG==False:
      print("Found product: "+result['Product'][0])
    global numProducts
    numProducts = numProducts + 1
    seriesDir = "defaultSeries"
    if len(result['Series']):
      seriesDir = result['Series'][0].split('|')
    global outputDir
    if len(result['Gender']):
      if result['Gender'][0]=="F":
        outputDir = outputDir + "Women\\"
      else:
        outputDir = outputDir + "Men\\"
    else:
      outputDir = outputDir + "defaultGender\\"
    size = len(seriesDir)
    i = 0
    for i in range(0, size):
      outputDir = outputDir + seriesDir[i].strip()+"\\"
    
    
    if len(result['ItemCode']):
      outputDir = outputDir +result['ItemCode'][0]+"\\"
    else:
      outputDir = outputDir + "defaultProduct\\"
    if DEBUG:
      print(outputDir)
    if os.path.isdir(outputDir)==False:
      try:
        os.makedirs(outputDir)
      except OSError as exception:
        print("Create output directory failed") 
    
    i = 0
    for item in result['Thumb']:
      urllib.urlretrieve(item, outputDir+result['ItemCode'][0]+"_thumb_"+str(i)+".jpg")
      i = i + 1
    #if len(result['Thumb']):
       #urllib.urlretrieve(result['Thumb'][0], outputDir+result['ItemCode'][0]+"_thumb.jpg")
    if len(result['HD']):
       urllib.urlretrieve(result['HD'][0], outputDir+result['ItemCode'][0]+"_HD.jpg")
    
    # write results to a file
    if len(result['ItemCode']):
      outfile = open(outputDir+result['ItemCode'][0]+".txt","w")
      delimiter = ", "
      for Key,Value in regex:
          outfile.write(Key+": ")
          size = len(result[Key])
          i = 0
          for i in range(0, size):
              if i<size-1:
                outfile.write(result[Key][i]+ delimiter)
              else:
                outfile.write(result[Key][i])
              i=i+1
          outfile.write("\n")
      outfile.close()


    if len(result['ItemCode']):
      request = urllib2.Request("http://www.gap.com/browse/productData.do?pid="+result['ItemCode'][0])
      request.add_header('User-Agent', 'Mozilla/5.0')
      opener = urllib2.build_opener()
      ajaxResponse = opener.open(request).read()
      ajaxResponseFile = open(outputDir+result['ItemCode'][0]+"_ajax.txt","w")
      ajaxResponseFile.write(ajaxResponse)
      ajaxResponseFile.close()


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

DEBUG = False
numProducts = 0
start = time.time()
brand = ["ARMANI","GAP","NIKE"]
outputDir = "D:\\"+brand[1]+"\\"
# the url to be processed
#url = "http://www.armani.com/us/ea7/short-sleeved-t-shirt_cod37583341dc.html"
#url = "http://www.armani.com/us"
url = "http://www.gap.com/browse/product.do?cid=1026525&vid=1&pid=227345002"
#http://www.gap.com/browse/productData.do?pid=227345
#url = "http://www.gap.com"
#url = "http://store.nike.com/us/en_us/pd/epic-lux-printed-running-tights/pid-10204487/pgid-10300783"

urlsUnvisited  = [url] # queue of urls to visit
urlsVisited    = [url] # queue of urls visited
singlePeriod = 0
while len(urlsUnvisited):
      singleStart = time.time()
      try:
          request = urllib2.Request(urlsUnvisited[0])
          request.add_header('User-Agent', 'Mozilla/5.0')
          opener = urllib2.build_opener()
          htmltext = opener.open(request).read()

          # r = Render(urlsUnvisited[0])  
          # htmltext = r.frame.toHtml()

          if DEBUG==False: # to see the original htmltext
            print("Output path: "+outputDir)
            if os.path.isdir(outputDir)==False:
              try:
                os.makedirs(outputDir)
                
              except OSError as exception:
                print("Create output directory failed")
            try: 
              outfile = open(outputDir+"htmltext.txt","w")
              outfile.write(htmltext)
              outfile.close()
              print("Htmltext file completed.")
            except:
              print("Cannot open htmltext.txt")

      except:
          print("ERROR: Cannot open: "+urlsUnvisited[0])

      urlparse.urlparse(urlsUnvisited[0])
      retrieveProductInfo(htmltext)
      soup = BeautifulSoup(htmltext)
      if DEBUG==False:
         print("Popping: "+urlsUnvisited[0])
      urlsUnvisited.pop(0)
      if DEBUG==False:
         print("Unvisited: "+str(len(urlsUnvisited)))
         print("Estimated time to finish: "+str('{0:.3g}'.format(singlePeriod*len(urlsUnvisited)/60))+" minutes")
      for tag in soup.findAll('a',href=True):
          tag['href'] = urlparse.urljoin(url,tag['href'])  #normalize

          if url in tag['href'] and tag['href'] not in urlsVisited:
             #urlsUnvisited.append(tag['href'])
             urlsVisited.append(tag['href'])
      singlePeriod = time.time()-singleStart
      if DEBUG:
        print("SinglePeriod costs: "+str(singlePeriod))+ " seconds"



print(str(len(urlsVisited))+" urls visited.")
print("It took "+ str('{0:.3g}'.format((time.time()-start)/60))+ " minutes.")
print(str(numProducts)+" products found.")


