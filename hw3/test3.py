import os
import sys
APP_NAME = "My Spark Application"

sentimentdic = {}
def extract_feature(content):
  def work(content, dic):
    coll = content.split("\t")
    col = coll[3].split(" ")
    res = [0.0,0.0]
    for w in col:
      if sentimentdic.has_key(w.lower()):
        if sentimentdic[w.lower()] == 1:
          res[0] = res[0] +1
        if sentimentdic[w.lower()] == -1:
          res[1] = res[1] + 1
    res[0] = res[0] / (1.0+len(col))      
    res[1] = res[1] / (1.0+len(col))
    ret = " ".join([str(x) for x in res])
    return ret
  return work(content, sentimentdic)

def load_dic(fn):
  fp = open(fn,"r")
  for s in fp:
    cols = s.rstrip().split()
    sentimentdic[cols[0]] = cols[1]

if __name__=="__main__":
  load_dic("words.txt")

  for s in sys.stdin:
    ss = extract_feature(s)
    print ss
  #df1=sc.parallelize(fr.collect()).saveAsTextFile("tweets/2013.txt")
  #df1.write.save("tweets2/2013.txt",format="text")
  #print df1
