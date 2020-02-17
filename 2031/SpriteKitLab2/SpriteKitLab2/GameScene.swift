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
    let backgroundNode2 = SKSpriteNode(imageNamed: "Background")
    let playerNode = SKSpriteNode(imageNamed: "Player")
    let enemyNode = SKSpriteNode(imageNamed: "Enemy")
    let labelNode = SKLabelNode(text: "0")
    
    var backgroundY: CGFloat = 0.0
    var backgroundY2: CGFloat = 0.0
    var label = 0
    
    
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
        
        backgroundNode2.size.width = frame.size.width
        backgroundNode2.anchorPoint = CGPoint(x: 0.5, y: 0.0)
        backgroundNode2.position = CGPoint(x: size.width / 2, y: backgroundNode2.size.height)
        addChild(backgroundNode2)


        let scroll = SKAction.move(by: CGVector(dx: 0.0, dy: -size.height*2), duration: 10)
        let reset = SKAction.move(by: CGVector(dx: 0.0, dy: size.height*2), duration: 0)
        let sequence = SKAction.sequence([scroll, reset])
        let InfScroll = SKAction.repeatForever(sequence)
        backgroundNode.run(InfScroll)
        
        let scroll2 = SKAction.move(by: CGVector(dx: 0.0, dy: -size.height*2), duration: 10)
        let reset2 = SKAction.move(by: CGVector(dx: 0.0, dy: size.height*2), duration: 0)
        let sequence2 = SKAction.sequence([scroll2, reset2])
        let InfScroll2 = SKAction.repeatForever(sequence2)
        backgroundNode2.run(InfScroll2)
        
        
        
        playerNode.position = CGPoint(x: size.width / 2, y: 80.0)
        addChild(playerNode)
        let playerMove = SKAction.moveBy(x: 0.0, y: size.height, duration: 10)
        let playerReset = SKAction.moveBy(x: 0.0, y: -size.height, duration: 0)
        let pSeq = SKAction.sequence([playerMove, playerReset])
        let InfP = SKAction.repeatForever(pSeq)
        playerNode.run(InfP)

        


        enemyNode.position = CGPoint(x: size.width / 2, y: 600)
        enemyNode.size = CGSize(width: 60.0, height: 60.0)
        addChild(enemyNode)
        let Erotate = SKAction.rotate(byAngle: 90, duration: 10)
        let Escale = SKAction.scale(by: 8, duration: 5)
        let Escale2 = SKAction.scale(to: 1, duration: 5)
        let Ecolor = SKAction.colorize(with: UIColor.purple, colorBlendFactor: 0.8, duration: 5)
        let Egroup = SKAction.group([Erotate,Escale,Escale2,Ecolor])
        let EInf = SKAction.repeatForever(Egroup)
        enemyNode.run(EInf)
        
        
        
        labelNode.position = CGPoint(x: 50, y: size.height - 50)
        addChild(labelNode)
        
    }
    
    var flag = false

    
    override func update(_ currentTime: TimeInterval){
        let followPlayer = SKAction.move(to: playerNode.position, duration: 1)
        enemyNode.run(followPlayer)
        
        if  hypotf(Float(enemyNode.position.x) - Float(playerNode.position.x), Float(enemyNode.position.y) - Float(playerNode.position.y)) <= 10 && flag == false  {
            label+=2
            flag = true
        }
        if hypotf(Float(enemyNode.position.x) - Float(playerNode.position.x), Float(enemyNode.position.y) - Float(playerNode.position.y)) >= 10 {
            flag = false
        }
        labelNode.text = String(label)
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        let pScale = SKAction.scale(by: 3, duration: 1)
        let pToGreen = SKAction.colorize(with: UIColor.green, colorBlendFactor: 0.4, duration: 1)
        let fadeAlpha = SKAction.fadeAlpha(to: 0.0, duration: 1)
        let fadeAlpha2 = SKAction.fadeAlpha(to: 1.0, duration: 1)
        let pScale2 = SKAction.scale(to: 1, duration: 1)
        let pSeq2 = SKAction.sequence([pScale,pToGreen,fadeAlpha, fadeAlpha2, pScale2])
        playerNode.run(pSeq2)
    }
    
    

}
