//
//  main.swift
//  A1
//
//  Created by CccrizzZ on 2/14/20.
//  Copyright © 2020 CccrizzZ. All rights reserved.
//

import Foundation


var score = 0

var a:String?

let Dictionary1 = [
    "What is my name?": "Chris",
    "What is my profession?": "programmer",
    "What is my school's name?": "GBC",
    "What is my hobby?": "music",
    "Witch city am I living in?": "Toronto",
    "What color is apple cider": "gold",
    "When did GTA 5 came out?": "2013",
    "What language is this program written in?": "Swift",
    "What year did the Chinese invented coronavirus?": "2018",
    "What animal appears on Canadian driver test?": "reindeer",
    "Whats the last name of current U.S. president?": "Trump",
    "Whats the last name of current Canadian Priminister?": "Trudeau",
    "Who is Tesla's CEO?": "Elon Musk",
    "Witch province in Canada have the most population?": "Ontario",
    "Witch country produce the most avocado": "Mexico",
    "Witch game's main charactor have a crowbar and a orange suit?": "Half Life",
    "What is Drake's brand name?": "OVO",
    "Witch country invented computer?": "USA",
    "What animal appears on Razer's logo?": "snake",
    "What animal appears on Github's logo?": "octopus",
    "Witch console have more sales? Xbox one or PS4?": "PS4",
    "What language is used for Unreal Engine 4?": "C++",
    "What language is used for Databases?": "SQL",
    "What is the code of our school program?": "T163",
    "Witch game is a sandbox game based on source engine?": "gmod",
    "What is OSX's default digital audio workstation?": "Garage band",
    "Who invented IPhone?": "Steve Jobs",
    "Witch prime minister is on the 100 Canadian dollar": "Robert Bordan",
    "What is Canada's iconic dish called?": "poutine",
    "Witch province of Canada speaks more French?": "Montreal"
]

print("Quiz Start")
print("=====================")

for (key, item) in Dictionary1 {
    print(key)
    
    a = readLine()
    

    if item == a {
        print("correct +10 points")
        score+=10
        print("Score: \(score)" + "\n")
    }else{
        print("incorrect, 1 more chance" + "\n")
        a = readLine()
        if item == a {
            print("correct +10 points")
            score+=10
            print("Score: \(score)" + "\n")
        }else{
            print("incorrect..." + "\n")
        }
    }
}

print("Final Score: \(score)")
