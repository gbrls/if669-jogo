import  pygame, math

pygame.init()
size = width, height = 800, 800
bg_color=50,0,0
fg_color = 0,0,255
h_color=255,255,255
fov=0.4
distance=100
nrays=10
screen=pygame.display.set_mode(size)


#x,y,rotation
player = [10, 20, 0]

def frange(start, stop=None, step=None):
    if stop == None:
        stop = start + 0.0
        start = 0.0

    if step == None:
        step = 1.0

    while True:
        if step > 0 and start >= stop:
            break
        elif step < 0 and start <= stop:
            break
        yield ("%g" % start)
        start = start + step

def proj(ang, idx):
    #d=p[1]

    ang=float(ang)
    c=math.cos(ang) 

    pygame.draw.line(screen, h_color,
            (int(player[0]+(math.cos(player[2]+float(ang)))*distance/c),
            int(player[1]+(math.sin(player[2]+float(ang))*distance/c))),
            (int(player[0]),int(player[1])))

    y =  player[1]+(math.sin(player[2]+float(ang))*distance/c)
    x =  player[0]+(math.cos(player[2]+float(ang))*distance/c)
    
    r=1000

    if y < 0:
        #rect(idx,abs(y))
        r=min(r,abs(y))

    if x < 0:
        #rect(idx,abs(x))
        r=min(r,abs(x))

    if r < 100:
        rect(idx,r)



def rect(idx,sz):
    pygame.draw.rect(screen,(sz,sz,sz), (idx * (width/nrays),400,(width/nrays),sz))

while(True):
    screen.fill(bg_color) 

    pygame.draw.circle(screen,fg_color,
            (int(player[0]), int(player[1])), 10)


    idx=0
    for i in frange(-fov,fov, (fov*2/nrays)):
        proj(i,idx)
        idx+=1


    for e in pygame.event.get():
        if e.type==pygame.QUIT: sys.exit()
        if e.type==pygame.KEYDOWN:
            if e.key==pygame.K_UP:
                player[0]+=math.cos(player[2])*20
                player[1]+=math.sin(player[2])*20
            if e.key==pygame.K_LEFT:
                player[2]-=0.5
            if e.key==pygame.K_RIGHT:
                player[2]+=0.5

    pygame.display.update()
