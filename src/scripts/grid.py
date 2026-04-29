import random

EPS = 1e-7

def grid(n):
    with open("grid.node", "w") as f:
        f.write(f"{n * n} 2 0 0\n")
        i = 1
        for x in range(n-1, -1, -1):
            for y in range(n):
                dx = random.uniform(-EPS, EPS)
                dy = random.uniform(-EPS, EPS)
                f.write(f"{i} {x + dx} {y + dy}\n")
                i += 1

grid(500)
