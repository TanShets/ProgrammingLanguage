class A inherits B{
    a = 5
    b = 4
    function constructor(c, d){
        this.c = c
        this.d = d
    }

    function constructor(x){
        this.c = x
        this.d = x
    }

    function m1(){
        return this.c
    }

    static function add_to_a(x){
        A.a += x
    }

    static function add_to_b(x){
        A.b += x
    }
}

a = A(5, 4)
c = a.m1()
print(c)
A.add_to_a(3)
A.add_to_b(5)
print(c)
print(A.a)
print(A.b)