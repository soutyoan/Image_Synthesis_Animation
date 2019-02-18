# Image Synthesis Animation

This is an Ensimag Project about Image Synthesis and Animation.

The goal is to make a render as realistic and fast as possible.

### How to compile the project for target myViewer

```console
me@machine:~$ qmake CONFIG+=myViewer
me@machine:~$ make
me@machine:~$ ./viewer/myViewer
```

### TP and challenges - Image Synthesis

- [x] TP1
- [x] TP2
- [x] TP3
- [x] Intéractivité
- [x] Shadow Mapping
- [ ] Bounding Volume Hierarchy (see bvh branch)
- [x] Rendu en alternance
- [ ] Caustiques

## Creating multiple targets with qmake

We are currently using multiple target for test purposes.  
- To change target :
 - Clean the folder by using the clean.sh file
 - Choose your target from one of the current available targets.
   Current targets are :
    1. myViewer (global)
    2. parser (parser test)
    3. tests (tests unitaires)

    Execute : qmake CONFIG+=target
