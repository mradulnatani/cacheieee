
# cacheieee

**cacheieee** is a lightweight, blazing-fast in-memory key-value data store written in C.
Inspired by Redis, CacheMe provides essential caching capabilities and gives developers a
behind-the-scenes look at how high-performance caching systems are built — from TCP networking
and socket management to memory storage and command parsing.

---

## Features

- **Basic key-value operations**: `SET`, `GET`, `DEL`, `EXISTS`
- **Key expiration with TTL support**: `SETEX`, `TTL`
- **Pattern matching for keys**: `KEYS`
- **Atomic integer operations**: `INCR`, `DECR`
- **Multi-client support** using TCP sockets
- **Tree-based key-value storage** for efficient lookups (in progress)
- **Clean command parsing** for simplicity and efficiency
- **Comprehensive CLI testing support**
- Built from scratch in **C** for learning systems-level programming

---

## Supported Commands

| Command                | Description                                                                |
|------------------------|----------------------------------------------------------------------------|
| `SET key value`         | Sets the value for the given key                                          |
| `GET key`               | Retrieves the value associated with the key                               |
| `DEL key`               | Deletes the key and its value                                             |
| `EXISTS key`            | Checks if the key exists in the store                                     |
| `KEYS pattern`          | Returns all keys matching the given pattern (e.g., `*`, `user*`, etc.)    |
| `SETEX key ttl value`   | Sets a value with a TTL (in seconds)                                      |
| `TTL key`               | Returns the remaining time to live (TTL) for a key                        |
| `INCR key`              | Increments an integer value by 1                                          |
| `DECR key`              | Decrements an integer value by 1                                          |
| `PING`                  | Health check — responds with `PONG`                                       |
| `FLUSHALL`              | Deletes all keys in the current database (dangerous!)                     |
| `INFO`                  | Returns server stats (upcoming)                                           |
| `QUIT`                  | Closes the client connection gracefully                                   |

---

##  Getting Started

### Prerequisites

- GCC or Clang compiler
- Make
- Linux/macOS (Windows is not officially supported)

### Build Instructions

1. Clone the repository:
    ```bash
    git clone https://github.com/mradulnatani/cacheieee.git
    ```
2. Navigate into the project directory:
    ```bash
    cd cacheieee
    ```
3. Build the project using `make`:
    ```bash
    make
    ```

---

## 🚀 Running the Server

To start the **CacheMe** server, simply run the following command:

```bash
./cacheieee
```

This will start the server on the default port **12049**.

---

## 🔗 Connecting to the Server

You can use tools like **telnet**, **netcat** (`nc`), or even your own client to connect to the server:

```bash
nc localhost 12049
```

Once connected, you can issue the supported commands directly:

```bash
SET name CacheMe
GET name
SETEX tempkey 10 hello
TTL tempkey
```

---


## 📈 Future Roadmap

Here are some features planned for future releases:

- **Snapshot-based persistence** (SAVE, LOAD)
- **Eviction policies** (LRU, LFU, etc.)
- **Pub/Sub system**
- **Clustering and replication**
- **Role-based authentication**
- Admin dashboard (via WebSocket + React?)

---

##  Contributing

Contributions, bug reports, and feature requests are welcome! Please feel free to fork the repository and submit a pull request.

---

## 📜 License

MIT License

---

## 🤝 Acknowledgements

- **Redis and CodeCrafters** for the idea and inspiration
- **Linux man pages** for socket programming guidance
- **C programming GOAT** Dr. Jonas Brich

---

## 📞 Contact

Made by **Mradul**  
Project port: `12049` | Protocol: TCP  

**CacheMe** if you can 
```

---

Feel free to copy this directly into your `README.md` file. Let me know if you need further modifications!
