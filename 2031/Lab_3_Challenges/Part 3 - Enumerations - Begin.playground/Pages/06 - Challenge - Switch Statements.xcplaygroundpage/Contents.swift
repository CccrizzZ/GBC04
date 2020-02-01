//: [⇐ Previous: 05 - More Switch Statements](@previous)
//: ## Episode 06: Challenge - Switch Statements

/*:
 ## Challenge 1
 - Write a switch statement that switches on a tuple containing a `String` and an `Int`. The `String` is a name, and the `Int` is an age.
 - Use the cases below (or make up your own), and binding with `let` syntax, to assign the the name, followed by the life stage related to the age, to a `String` constant.
 
 For example, for the author of these challenges, you'd assign "Matt is an adult." to your constant.
 
 * 0-2 years: Infant
 * 3-12 years: Child
 * 13-19 years: Teenager
 * 20-39: Adult
 * 40-60: Middle aged
 * 61+: Elderly
*/

var Mark = (name:"Mark", age:2, lifestage:String())
var Tim = (name:"Tim", age:10, lifestage:String())
var Max = (name:"Max", age:19, lifestage:String())
var Chris = (name:"Chris", age:21, lifestage:String())
var John = (name:"John", age:42, lifestage:String())
var Ian = (name:"Ian", age:63, lifestage:String())


var temp = Max

switch temp.age {
case 0...2:
    temp.lifestage = "Infant"
case 3...12:
    temp.lifestage = "Child"
case 13...19:
    temp.lifestage = "Teenager"
case 20...39:
    temp.lifestage = "Adult"
case 40...60:
    temp.lifestage = "Middle Age"
case 61...:
    temp.lifestage = "Elderly"
default:
    print("Please enter the correct age")
}


print("Name: " + temp.name + ", Lifestage: " + temp.lifestage)





/*:
 ## Challenge 2
 Imagine starting a new level in a video game. The character makes a series of movements in the game. Calculate the position of the character on a top-down level map after making a set of movements.
 - Create a `Direction` enumeration with cases for `north`, `south`, `east`, and `west`.
 - Write a function that takes a `Direction` array and returns a tuple of two Ints representing the character's location after all of the movements.
 - Assume the character starts at (0, 0)
 
 Example: A series of movements like [.north, .west, .west] would return a location of (-2, 1)
*/

var position = (x:0,y:0)

enum Direction{
    case north, south, east, west
}

func InputMovement(dir:Direction){
    switch dir {
    case .north:
        position.y+=1
    case .south:
        position.y-=1
    case .east:
        position.x+=1
    case .west:
        position.x-=1
    }
    print("Player Location: (" , position.x , ", " , position.y, ")")
}

InputMovement(dir: Direction.north)
InputMovement(dir: Direction.north)
InputMovement(dir: Direction.south)
InputMovement(dir: Direction.west)

//: [⇒ Next: 07 - Associated Values](@next)
