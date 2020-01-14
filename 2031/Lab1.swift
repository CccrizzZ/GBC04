// // declaration of variable
// var a = 1
// a = 2


// // declaration of constant
// let b = 1
// // b = 2 //error

// let sum = a + b
// print("sum = \(sum)")
// let diff = 5 - sum
// print("diff = \(diff)")

// Exercise 1
var secondsInYear = 365 * 24 * 3600
print("secondsInYear = \(secondsInYear)")


// Exercise 2
var width = 1920
var height = 1080

var numberOfPixels = width * height
var resolution = "\(width) x \(height)"

print("numberOfPixels = \(numberOfPixels)")
print("resolution = \(resolution)")


// Exercise 3
func IsContaining(InputArray: [String], Keyword: String) -> Bool {
    for str in InputArray {
        if str == Keyword {
            return true;
        }
    }
    return false;
}

var ShoppingList = ["CrawFish", "Black Pepper", "Corn", "Beer", "Lemon", "Bread"]

print("Is Corn in my shopping list?")
print(IsContaining(InputArray: ShoppingList, Keyword: "Corn"))
print("Is Beef in my shopping list?")
print(IsContaining(InputArray: ShoppingList, Keyword: "Beef"))
print("Is Beer in my shopping list?")
print(IsContaining(InputArray: ShoppingList, Keyword: "Beer"))
