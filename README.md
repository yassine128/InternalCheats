<h1>Internal Cheats</h1>

## `✋` IMPORTANT
> **Warning**: *The code in this repo is is explicitly for educational purposes. I am not responsible for anything that happens when you use this software. *

This repository was created to archive and document all of my attempts to learn and develop cheats. This is a beginner friendly guide. 

- `🟢` Infinite Health 
- `🟢` Radar
- `🟡` ESP 

## `📚` PREREQUISITES
In order to effectively follow this guide, it is necessary to possess prior experience in C++, Assembly and a comprehensive understanding of computer memory concepts.

# Finding offsets 
Offsets in the context of computer memory refer to the distances between the starting points of different memory locations. 
Each memory location has a unique address, and the offset represents the difference between the address of a specific memory location and the reference point, often the starting address.

Offsets are commonly used to access and manipulate data in computer memory. By adding an offset value to a base memory address we can access an attribute of a struct/class. 

To better understand, let's look at an example. 

```c++
struct player {
    int health; // offset: 0x0
    int shield; // offset: 0x4
    float x;    // offset: 0x8
    float y;    // offset: 0x12
    float z;    // offset: 0x16
}

player* myPlayer{100, 0, 1, 2, 3}; 
```
In this example, we have a pointer to a player. In a software like [Cheat Engine](https://cheatengine.org/) we can find the static pointer value which often looks like `ac_client.exe + 0x195404`, where `ac_client.exe` is the base adress of the executable and `0x195404` is the offset to get the local player. You can then simply add the offset to any attribute of the struct to get the value you want. In C++, the code to access the value looks like this: 

```c++
DWORD_PTR GetExecutableBaseAddress()
{
    HMODULE hModule = GetModuleHandleA("ac_client.exe");  
    return (DWORD_PTR)hModule;
}

void findMyPlayer() {
    DWORD_PTR baseAddress = GetExecutableBaseAddress();
    auto localPlayer = reinterpret_cast<std::uintptr_t*>(baseAddress + offsets::dwLocalPlayer);
    myPlayer = new playerObject{
        reinterpret_cast<int*>(*localPlayer + offsets::pHealth),
        reinterpret_cast<float*>(*localPlayer + offsets::pSouthNorth),
        reinterpret_cast<float*>(*localPlayer + offsets::pWestEast),
        reinterpret_cast<char*>(*localPlayer + offsets::pName),
        reinterpret_cast<float*>(*localPlayer + offsets::viewAngle)
    };
}
```
Once you have all the attribute you want, it's good to create a custom struct to store the entity. 

```c++
struct playerObject {
    int* health;
    float* southPos;
    float* westPos;
    char* name;
    float* viewAngle; 

    std::string getPlayerDetails() {
        std::stringstream ss;
        ss << "Name: " << name << "\n";
        ss << "Health: " << std::to_string(*health) << "\n";
        ss << "South-North Position: " << std::to_string(*southPos) << "\n";
        ss << "West-East Position: " << std::to_string(*westPos) << "\n";
        return ss.str();
    }
};
```


