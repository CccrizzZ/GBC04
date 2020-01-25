//: Episode 06: Challenge - Arrays
/*:
 ## Challenge 1
 
 Using the `players` array below, and the array methods and data you learned about in this chapter, determine the following things about the array:
 
 (a) The count of elements in the array
 (b) Whether or not the array contains the String value "Charles"
 (c) The first element in the array
 (d) The last element in the array
 
 */

var players = ["Alice", "Bob", "Dan", "Eli", "Frank"]

// a
print("The count of elements is", players.count)
// b
var a = false;
for item in players {
    if item == "Charles" {
        a = true;
    }
}
print("Find Charles? " + (a ? "Yes" : "No"))
// c
print("First element: " + players[0])
// d
print("Last element: ", players[(players.count)-1])




/*:
 ## Challenge 2
 
 Some new players have joined the game: Charles, Gloria and Hermione. You'd like to add them to the array; Gloria and Hermione at the end, and Charles somewhere in the middle.
 
 (a) Insert Charles at index 2 in the array.
 (b) Add Gloria and Hermione at the end of the array in a single line of code.
 */

// a
players.insert("Charles", at:2)
// b
players.insert("Gloria", at:(players.count))
players.insert("Hermione", at:(players.count))
print("The new array looks like this: ")
for item in players {
    print(item)
}


/*:
 ## Challenge 3
 
 Create another new constant array named `teamOne` that consists of the last four members of the `players` array; that would be the range of elements from 4...7
 */

var teamOne: Array<String?> = []

var i:Int = players.count-1

while i >= 4 {
    teamOne.append(players[i])
    i-=1
}

print("\nTeam One :")

for item in teamOne {
    print(item!)
}


