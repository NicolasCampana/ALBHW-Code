//
// Created by nicolas on 27/04/19.
//

#ifndef NICOLAS_IC_UTIL_H
#define NICOLAS_IC_UTIL_H


class Util {
public:
    vector<string> static getMediumPaths(int i) {
        vector<string> instances;
        DIR *dir;
        struct dirent *ent;
        vector<string> path = Util::addMediumPaths();
        if ((dir = opendir (path[i].c_str())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                // Enquanto houver instancias para serem lidas
                if(strcmp(ent->d_name, "..") != 0 and strcmp(ent->d_name, ".") != 0) {
                    instances.push_back(ent->d_name);
                }
            }
            sort(instances.begin(), instances.end());
        }
        return instances;
    }
    vector<string> static getSmallPaths(int i) {
        vector<string> instances;
        DIR *dir;
        struct dirent *ent;
        cout << " PRORA ";
        vector<string> path = Util::addSmallPaths();
        if ((dir = opendir (path[i].c_str())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                // Enquanto houver instancias para serem lidas
                if(strcmp(ent->d_name, "..") != 0 and strcmp(ent->d_name, ".") != 0) {
                    instances.push_back(ent->d_name);
                }
            }
            sort(instances.begin(), instances.end());
        }

        return instances;
    }

    vector<string> static addSmallPaths() {
        std::vector<std::string> path;
        path.push_back("Instances/Small/Instance1.1-0.7/");
        path.push_back("Instances/Small/Instance1.1-0.85/");
        path.push_back("Instances/Small/Instance1.2-0.7/");
        path.push_back("Instances/Small/Instance1.2-0.85/");
        return path;
    }

    vector<string> static addMediumPaths() {
        std::vector<std::string> path;
        path.push_back("Instances/Medium/Instance1.1-0.7/");
        path.push_back("Instances/Medium/Instance1.1-0.85/");
        path.push_back("Instances/Medium/Instance1.2-0.7/");
        path.push_back("Instances/Medium/Instance1.2-0.85/");
        return path;
    }

};


#endif //NICOLAS_IC_UTIL_H
