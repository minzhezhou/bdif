import re
import csv

f = open('LoughranMcDonald_MasterDictionary_2014.csv', 'rb')
reader = csv.reader(f)
headers = reader.next()

wordlist = {}
for h in headers:
    wordlist[h] = []
    
for row in reader:
    for h, v in zip(headers, row):
        wordlist[h].append(v)
f.close()

fp = open("words.txt","w")
word_list={}
for i in range(len(wordlist['Positive'])):
    temp = 0
    if wordlist['Positive'][i]=='2009':
        temp=1
    elif wordlist['Negative'][i]=='2009':
        temp=-1
    if temp:
      word_list[wordlist['Word'][i].lower()]=temp
      fp.write(wordlist['Word'][i].lower() + "\t" + str(temp) + "\n")

print len(word_list)
fp.close()
exit(0)
