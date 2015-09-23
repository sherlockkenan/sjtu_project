
from urllib import urlopen
from bs4 import BeautifulSoup
import csv


html_federer = urlopen('http://cn.atpworldtour.com/Tennis/Players/Top-Players/Roger-Federer.aspx?t=mr')
obj_f=BeautifulSoup(html_federer.read(),'html.parser')

html_nadal=urlopen('http://cn.atpworldtour.com/Tennis/Players/Top-Players/Rafael-Nadal.aspx?t=mr')
obj_n=BeautifulSoup(html_nadal.read(),'html.parser')

divs_fw=obj_f.findAll(attrs={"class":"careerWon"},limit=20)
divs_fl=obj_f.findAll(attrs={"class":"careerLost"},limit=20)
divs_fr=obj_f.findAll(attrs={"class":"careerReliability"},limit=20)

divs_nw=obj_n.findAll(attrs={"class":"careerWon"},limit=20)
divs_nl=obj_n.findAll(attrs={"class":"careerLost"},limit=20)
divs_nr=obj_n.findAll(attrs={"class":"careerReliability"},limit=20)


with open('数据.csv','wb')as file:
 writer=csv.writer(file,dialect='excel')
 
 writer.writerow(['','费德勒','','','纳达尔'])
 writer.writerow(['','胜','负','胜率','胜','负','胜率'])

 i=13
 place=['硬地赛事','红土赛事','草地赛事','地毯赛事','室内赛事','室外赛事']
 while(i<19):
   writer.writerow([place[i-13],divs_fw[i].get_text().encode("utf-8"),
                    divs_fl[i].get_text().encode('gb2312','ignore'),
                    divs_fr[i].get_text().encode('gb2312','ignore'),
                    divs_nw[i].get_text().encode('gb2312','ignore'),
                    divs_nl[i].get_text().encode('gb2312','ignore'),
                    divs_nr[i].get_text().encode('gb2312','ignore')])
   i=i+1

   
 writer.writerow([''])
 writer.writerow(['获得的冠军个数'])
 writer.writerow(['','费德勒冠军','纳达尔冠军'])
 
 year=['2003','2004','2005','2006','2007','2008','2009','2010','2011','2012','2013','2014','2015']
 for j in  year :
    html2_federer = urlopen('http://cn.atpworldtour.com/Tennis/Players/Top-Players/Roger-Federer.aspx?t=pa&y='+j+'&m=s&e=0#')
    obj2_f=BeautifulSoup(html2_federer.read(),'html.parser')
    divs_f=obj2_f.find(attrs={"class":"bioTableAlt centerCells"})
    st_f=divs_f.findAllNext('td',limit=5)[4].get_text()
  

    html2_nadal = urlopen('http://cn.atpworldtour.com/Tennis/Players/Top-Players/Rafael-Nadal.aspx?t=pa&y='+j+'&m=s&e=0#')
    obj2_n=BeautifulSoup(html2_nadal.read(),'html.parser')
    divs_n=obj2_n.find(attrs={"class":"bioTableAlt centerCells"})
    st_n=divs_n.findAllNext('td',limit=5)[4].get_text()
 
    writer.writerow([j,st_f.encode('gb2312','ignore'),st_n.encode('gb2312','ignore')])


 html2 = urlopen('http://cn.atpworldtour.com/Players/Head-To-Head.aspx?pId=N409&oId=F324')
 soup=BeautifulSoup(html2.read(),'html.parser')
 div=soup.find('table',attrs={"class":"bioTableAlt stripeMe centerCells"})

 writer.writerow([''])
 writer.writerow(['对战的数据'])
 writer.writerow(['年份','场地','胜者'])
 
 i=3
 while(True):
  
   writer.writerow([div.contents[1].contents[i].contents[1].get_text(),
                    div.contents[1].contents[i].contents[5].get_text().encode('gb2312'),
                    div.contents[1].contents[i].contents[9].contents[0].get_text().encode('gb2312','ignore')])

   if(div.contents[1].contents[i].contents[1].get_text()=='2004'):
     break
   i=i+2
