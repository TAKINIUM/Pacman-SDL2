```mermaid
flowchart LR
    Start([Update Pacman]) --> Dir[Changement Direction]
    Dir --> Wall{Mur Devant ?}
    
    Wall -- Oui --> Block[Bloque]
    Wall -- Non --> Move[Avancer et Tunnel]
    
    Move --> Eat{Sur un point ?}
    Eat -- Oui --> Score[Manger et Score]
    
    Block & Score & Eat --> Anim[Animation Sprite]
```