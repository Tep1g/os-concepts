main() {
    //parent process a
    if (fork() == 0) {
        //child process b
        if (fork() == 0) {
            //subchild process d
            exit();
        }
        wait();
        exit();
    }
    if (fork() == 0) {
        //child process c
        exit();
    }
    
    for(int i=0; i<2; i++) {
        wait();
    }
}

main() {
    int codes[10];
    int file_descriptor[10][2];

    for(int i=0; i<10; i++) {
        pipe([file_descriptor[i]]);
        write(file_descriptor[i][1], codes[i], sizeof(int))
        if(fork() == 0) {
            //spawn child process

            int code_from_parent;
            read(file_descriptor[i][0], &code_from_parent, sizeof(int));
            //do stuff with code_from_parent
            
            exit();
        }
    }

    for(int i=0; i<10; i++) {
        wait();
    }
}