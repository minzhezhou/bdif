dica={"a":1,"b":2,"c":3}

def func1(str):
  ret = []
  for s in str:
    ret.append(dica[s])
  print ret

func1("bca")  
