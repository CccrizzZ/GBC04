//: ## Episode 08: Challenge - Classes

/*:
 # Challenge 1

 Convert the `Student` struct below to a class!
 You'll need to:
 1) Use the `Class` keyword
 2) Creat an initializer (Hint: Use the keyword `init`)
 3) Fix up the `mutating` method
 4) Make `chris` a constant
*/
class Student {
    // Member variables
    let name: String
    var grade: Int
    var pet: String?
    
    // Initiallizer
    init( name:String, grade:Int, pet:String?) {
       self.name = name
       self.grade = grade
       self.pet = pet
    }

    func getPassStatus(lowestPass: Int = 50) -> Bool {
        return grade >= lowestPass
    }

    func earnExtraCredit() {
        grade += 50
    }
}


let Chris = Student(name: "Chris", grade: 0, pet:"Mango")


print(Chris.getPassStatus())
Chris.earnExtraCredit()
print(Chris.getPassStatus())




//------------------------------------------------
//struct Student {
//  let name: String
//  var grade: Int
//  var pet: String?
//
//  func getPassStatus(lowestPass: Int = 50) -> Bool {
//    grade >= lowestPass
//  }
//
//  mutating func earnExtraCredit() {
//    grade += 10
//  }
//}
//
//var chris = Student(name: "Chris", grade: 49, pet: "Mango")
//chris.getPassStatus()
//chris.earnExtraCredit()
//chris.getPassStatus()
//------------------------------------------------

