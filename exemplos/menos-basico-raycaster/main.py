import pygame, sys, math

size = width, height = 800, 800
pygame.init()
screen=pygame.display.set_mode(size)

pX=2
pY=2
pAng=0.0
fov=math.pi/3

mapWidth=20
mapHeight=6

game = ""
game += "####################"
game += "#..........#.......#"
game += "#..................#"
game += "#..........#.......#"
game += "#..........#.......#"
game += "####################"

def draw():


    for i in range(0,int(height/2),10):
        c = (i/3)%255
        pygame.draw.rect(screen,(c,c,c),(0,i + height/2,width,10))

    for i in range(width):
        rAng = (pAng-fov)/2 + (float(i)/float(width))*fov

        distWall=0
        hitWall=False

        maxDepth=20
        
        sp = 10

        while not hitWall and distWall<maxDepth:
            #distWall += 0.1/math.cos(pAng) 
            distWall += 0.1

            testX=int(pX + math.sin(rAng)*distWall)
            testY=int(pY + math.cos(rAng)*distWall)

            oldX=int(pX + math.sin(rAng)*(distWall-0.1))
            oldY=int(pY + math.cos(rAng)*(distWall-0.1))



            if testX < 0 or testX >= mapWidth or testY < 0 or testY >= mapHeight:
                hitWall=True
                #distWall=maxDepth
            else:
                #if game[oldX + testY*mapWidth]=='#' or game[testX + oldY*mapWidth]=='#':
                    #sp=100

                if game[testX + testY*mapWidth]=='#':
                    hitWall=True

                    blockX=float(testX)+0.5
                    blockY=float(testY)+0.5

                    lookX=pX + math.sin(rAng)*distWall
                    lookY=pY + math.cos(rAng)*distWall

                    angle = math.atan2((lookY-blockY),(lookX-blockX))

                    if angle < -math.pi*0.25 and angle > -math.pi*0.75:
                        #sp=100
                        pass
                    #if angle >= math.pi*0.75 or ange < -math.pi*0.75:
                        #sp=100



        teto = (100/(distWall))

        pygame.draw.rect(screen,(min(teto,255),sp,10),(i,height/2-teto,2,teto*2))


def debug():

    pygame.draw.rect(screen, (100,100,10,15),(0,0,mapWidth*5,mapHeight*5))

    for i in range(mapWidth):
        for j in range(mapHeight):
            if game[i+j*mapWidth]=='#':
                pygame.draw.rect(screen, (10,100,10,30),(i*5,j*5,5,5))

    pygame.draw.circle(screen,(10,10,100,50),(int(pX*5),int(pY*5)),5)
    pygame.draw.line(screen,(255,255,255),(int(pX*5),int(pY*5)),
            (int(pX*5+math.cos(pAng)*-10),int(pY*5+math.sin(pAng)*10)))

while(True):
    screen.fill((100,100,100))
    for e in pygame.event.get():
        if e.type==pygame.QUIT: sys.exit()
    keys_pressed = pygame.key.get_pressed()

    if keys_pressed[pygame.K_LEFT]:
        pAng -= 0.1
    if keys_pressed[pygame.K_RIGHT]:
        pAng += 0.1

    if keys_pressed[pygame.K_UP]:
        nx = int(math.cos(pAng)*0.1 + pX)
        ny = int(math.sin(pAng)*0.1 + pY)

        if not (nx < 0 or nx >= mapWidth or ny < 0 or ny >= mapHeight or game[nx + ny*mapWidth]=='#'):
            pX -= math.cos(-pAng)*0.1
            pY -= math.sin(-pAng)*0.1


    draw()
    debug()

    pygame.display.update()
