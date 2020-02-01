//: [⇐ Previous: 02 - Inheritance](@previous)
//: ## Episode 03: Challenge - Inheritance

/*:
## Challenge 1 - Building a Class Heirarchy
Create a class named `Animal` that has…
1. a function named `speak()` that does nothing.
 
Create two `Animal` subclasses...
1. `WildAnimal` that...
  - has an `isPoisonous` property, that is a `Bool`
  - has an initializer that takes `isPoisonous` as a parameter
2. `Pet` that...
  - has a stored property named `name`, that is a `String`
  - has an initializer that takes `name` as a parameter
  - has a `play()` method that prints out a message
  - overrides `speak()` - It should print out a message

Create one subclass of your choice of `WildAnimal` or `Pet`. It should do at least one of the following:
 - override `speak()`
 - override `play()`
 - Add a new computed property
 - Add a new method
*/


class Animal{
    func speak() -> Void{
    }
}


class WildAnimal : Animal {
    
    init(isPoisonous: Bool) {
        self.isPoisonous = isPoisonous
    }
    
    var isPoisonous: Bool
}

class Pet : Animal {
    init(name: String) {
        self.name = name
    }
    
    var name: String
    
    func play() {
        print("Playing with your pet")
    }
    
    override func speak() {
        print("Your pet start to speaking to you")
    }
    
}


class Dog : Pet {
    override func speak() {
        print("Oofff")
    }
    
    override func play() {
        print("Your dog start to runing around")
    }
    
    var FurColor: String = "Blue"
    
    func bath() {
        print("Took a shower for your dog")
    }
}



/*:
## Challenge 2 - Casting
- Create at least one instance of each class from the first challenge.
- Create an array that contains all of the instances.
- Write a function that takes an `Animal` and does something different depending on what subclass it is. Try using conditional downcasting!
- Call the function with each of your instances using a loop or whatever other method you'd like!
*/

let a = Animal()
let b = WildAnimal(isPoisonous: true)
let c = Pet(name: "c")
let d = Dog(name: "d")



func PrintSubClassName(a:Animal){
    print( a.self )
}

PrintSubClassName(a: b)
PrintSubClassName(a: a)
PrintSubClassName(a: c)
PrintSubClassName(a: d)
//: [⇒ Next: 04 - Initializers](@next)
