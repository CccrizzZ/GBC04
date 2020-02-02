//: [⇐ Previous: 07 - Protocols & Extensions](@previous)
//: ## Episode 08: Challenge - Protocols

//: [Previous](@previous)

/*:
 ## Challenge 1
 - Create a protocol `Shape` that defines a read-only property `area` of type `Double`.
 - Implement `Shape` with structs representing `Square`, `Triangle` and `Circle`.
 - Add a circle, a square and a triangle to an array. Convert the array of `Shape`s to an array of `area`s using map.
 
 **HINTS**
 - The area of a square = one of its sides squared
 - The area of a triangle = 0.5 multiplied by its base multiplied by its height
 - The area of a circle = pi multiplied by its radius squared
 - `pi` is a property of `Double`!
 - You can copy the extension on `Numeric` into this page and use the `squared` property!
*/


protocol Shape{
    var area: Double{get}
}

struct Square : Shape{
    var side: Double
    
    var area: Double{
        return side*side
    }
    
}

struct Triangle : Shape{
    var base: Double
    var height: Double
    var area: Double{
        return (height*base)/2
    }
}

struct Circle : Shape{
    var radians: Double
    var area: Double{
        return Double.pi*(radians*radians)
    }
}

let s = Square(side: 10)
let t = Triangle(base: 5, height: 10)
let c = Circle(radians: 2)

let newArr: [Shape] = [s,t,c]

let newArea = newArr.map{$0.area}

print(newArea)



/*:
 ## Challenge 2 Extending a Protocol
 Below is a function that takes a Double and tells you if it is an integer by comparing the Double to a rounded version of itself.
 - Turn that function into a computed property of `FloatingPoint` (a protocol that Double and other floating point numbers conform to!)
 - Try the property out on instances of `Double` and `Float`!
*/

// -----------------------------------
func isInteger(number: Double) -> Bool {
  number.rounded() == number
}
// -----------------------------------

extension FloatingPoint{
    func isInteger(number: Double) -> Void {
        print(number.rounded() == number)
    }
    func isInteger(number: Float) -> Void {
        print(number.rounded() == number)
    }
}




let temp: Double = 2.56
let temp2: Float = 2.3
let temp3: Double = 2

temp.isInteger(number: temp)
temp2.isInteger(number: temp2)
temp3.isInteger(number: temp3)
