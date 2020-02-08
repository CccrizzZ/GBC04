//
//  GameScene.swift
//  SuperSpaceMan
//
//  Created by CccrizzZ on 2/8/20.
//  Copyright © 2020 Apress. All rights reserved.
//

import SpriteKit

class GameScene: SKScene {
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    override init(size: CGSize) {
        super.init(size: size)
        //backgroundColor = SKColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.0)
        backgroundNode.size.width = frame.size.width
        backgroundNode.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode.position = CGPoint(x: size.width / 2, y: 0.0)
        addChild(backgroundNode)
        
        playerNode.position = CGPoint(x: size.width / 2, y: 80.0)
        addChild(playerNode)
    }
    
    
    let backgroundNode = SKSpriteNode(imageNamed: "Background")
    let playerNode = SKSpriteNode(imageNamed: "Player")
}
