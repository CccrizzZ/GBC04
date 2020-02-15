//
//  GameViewController.swift
//  SpriteKitLab2
//
//  Created by CccrizzZ on 2/15/20.
//  Copyright © 2020 Apress. All rights reserved.
//

import SpriteKit

class GameViewController: UIViewController {
    var scene: GameScene!
    
    override var prefersStatusBarHidden: Bool{
        return true
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Configure the main view
        let skView = view as! SKView
        skView.showsFPS = true
        
        // Create and configure our game scene
        scene = GameScene(size: skView.bounds.size)
        scene.scaleMode = .aspectFill
        
        // Show the scene
        skView.presentScene(scene)
    }
}
