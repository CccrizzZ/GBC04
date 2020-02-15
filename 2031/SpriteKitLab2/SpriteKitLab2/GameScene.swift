//
//  GameScene.swift
//  SpriteKitLab2
//
//  Created by CccrizzZ on 2/15/20.
//  Copyright © 2020 CccrizzZ. All rights reserved.
//

import SpriteKit

class GameScene: SKScene {
    
    let backgroundNode = SKSpriteNode(imageNamed: "Background")
    let playerNode = SKSpriteNode(imageNamed: "Player")
    let enemyNode = SKSpriteNode(imageNamed: "Enemy")
    let labelNode = SKLabelNode(text: "Lable")
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    
    override init(size: CGSize) {
        super.init(size: size)
        //backgroundColor = SKColor(red: 0.0, green: 0.5, blue: 0.5, alpha: 1.0)
        backgroundNode.size.width = frame.size.width
        backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPoint(x: size.width / 2, y: 0.0)
        addChild(backgroundNode)
        
        playerNode.position = CGPoint(x: size.width / 2, y: 80.0)
        addChild(playerNode)
        
        enemyNode.position = CGPoint(x: size.width / 2, y: 600)
        addChild(enemyNode)
        
        labelNode.position = CGPoint(x: 50, y: size.height - 50)
        addChild(labelNode)
        
    }

}
