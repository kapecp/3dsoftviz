class A
  prop1: 3
  prop2: 2
  method2: () => 
                  prop1=30
                  prop2=20
                  prop1+prop2, 50250
                 
  method1: (a) => 
          print @prop1 .. ", " .. @prop2
          {@prop1+a, 5}
         
  5+5
  1+2
  3+2
  method3: ->
  method4: => ,method5: (var) -> return var+1
  [2+2]: maybe_prop_weird

class B extends A
     new: => 
      @propB1 = 10
      @propB2 = "value"
     propB3: 90
     methodB1: (propB1) => 
            @propB4 = 67
            print(@propB1, propB1, @propB4)
     methodB2: => print @propB4
          

var1 = 1
var2 = 1

func1 = (num using var2) ->
 var2 = 10   --[[ toto zmeni predoslu hodnotu 1 ]]
 var1 = 20     -- lokalna premenna
 print "Inside func1: " .. num+var2+var1
 num+var2+var1

print var1, var2, func1(1000), var1, var2

instance1 = A(7)
instance2 = A!

result = instance1\method1(5)
{g,h} = result   -- g, h = result[1], result[2]
print "Values from table: " .. g, h

retVal1, retVal2 = instance1\method2()

print instance1\method1(1)[1]
print retVal1, retVal2

instance3 = B()
instance3\methodB2!
instance3\methodB1(44)
instance3\methodB2!


-- last line: return from module
{:A, a:A, A, :func_test}
