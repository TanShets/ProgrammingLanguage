class Z{
    function constructor(){
        this.w = 5
        print("Entered constructor of Z")
    }
}

class B inherits Z{
    x = 5

    function constructor(){
        this.c = 0
        Z()
        print(this.c)
        print("Entered constructor of B")
    }

    function getC(){
        return this.c
    }
}

class E{
    y = 5
}

class C inherits E{
    x = 10
}

class A inherits C, B{
    a = 5
    b = 4
    function constructor(c, d){
        print("Entered constructor of A")
        A(d)
        B()
        print("Exited B")
        this.c = c
        this.d = d
    }

    function constructor(x){
        this.e = x
    }

    function m1(){
        print("Entered m1")
        print(this.e)
        return this.c
    }

    static function add_to_a(x){
        print("Entered this", A.a)
        A.a += x
        print("Exit", A.a)
    }

    static function add_to_b(x){
        A.b += x
    }

    static function get_B_x(){
        return x
    }
}

print("Started")
a = A(5, 4)
print("Step 1")
c = a.m1()
print("Step 2")
print(c)
A.add_to_a(3)
print("Step 3")
A.add_to_b(5)
print("Step 4")
print(c)
print(A.a)
print(A.b)
print(B.x)
print(A.get_B_x())
print(a.getC())
print(a.e)
b = A(3, 3)
print(a.e, b.e)
a.add_to_a(5)
print("Finished this thing", A.a, a.a, b.a)
print(A.y)
print(a.B.c)
print("a.B.Z.w:", a.B.Z.w)