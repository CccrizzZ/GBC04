//: ## Episode 04: Challenge - Dictionaries

/*:
 
 ## Challenge 1
 
 Create a dictionary as a variable, and initialize it with the following keys:
 
 - `name`
 - `profession`
 - `country`
 - `city`
 
 As part of the initialization, assign each of those keys a value that corresponds to your own personal information.
 
 */

var Mydic = [
    "name": "Chris",
    "profession": "Music Producer",
    "country": "USA",
    "city": "Long Beach"
]

print(Mydic,"\n---------------------")



/*:
 
 ## Challenge 2
 
 Let's say you decide to move to Cleveland, Ohio, USA. (If you already live in Cleveland, pick somewhere else you'd like to move to!) Update the dictionary as follows:
 
 - Modify `country` to `USA`
 - Modify `city` to `Cleveland`
 - Add a `state` key to the dictionary and assign it the value `Ohio`
 
 */

Mydic["country"] = "USA"
Mydic["city"] = "Cleveland"
Mydic["state"] = "Ohio"

print(Mydic,"\n---------------------")



/*:
 
 ## Challenge 3
 
 You've decided that Cleveland (or wherever you've moved to) isn't right for you, so instead you plan to be a digital nomad, with no fixed city or state, but stay inside the USA.
 
 - Remove the `city` key-value pair with whatever strategy you like.
 - Remove the `state` key-value pair with a different strategy.
 
 */
Mydic["city"] = nil;
Mydic["state"] = nil;

print(Mydic,"\n---------------------")



/*:
 
 ## Challenge 4
 
 Iterate over the remaining keys and values in the dictionary and print them out.
 
 */


for item in Mydic {
    print(item.key + ":" + item.value)
}

