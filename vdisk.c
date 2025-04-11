#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/genhd.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>

#define SECTOR_SIZE 512
#define DISK_SIZE_SECTORS 102400

static struct gendisk *vdisk;
static struct request_queue *queue;
static u8 *storage;

// Обработчик запросов
static void vdisk_request(struct request_queue *q) {
    struct request *req;
    while ((req = blk_fetch_request(q)) != NULL) {
        // Получение параметров запроса
        unsigned long start = blk_rq_pos(req) * SECTOR_SIZE;
        unsigned long len = blk_rq_bytes(req);
        char *buffer = bio_data(req->bio);

        // Обработка операции
        switch(req_op(req)) {
            case REQ_OP_READ:
                memcpy(buffer, storage + start, len);
                break;
            case REQ_OP_WRITE:
                memcpy(storage + start, buffer, len);
                break;
            case REQ_OP_DISCARD:
                memset(storage + start, 0, len);
                break;
            default:
                pr_err("Unsupported operation\n");
        }

        __blk_end_request_all(req, 0);
    }
}

static int __init vdisk_init(void) {
    storage = vmalloc(DISK_SIZE_SECTOR * SECTOR_SIZE);
    if (!storage) return -ENOMEM;

    // СОзданип блочного устройства
    vdisk = alloc_disk(1);
    if (!vdisk) {
        vfree(storage);
        return -ENOMEM;
    }

    // Инициализация очереди запросов
    queue = blk_init_queue(vdisk_request, NULL);
    if (!queue) {
        put_disk(vdisk);
        vfree(storage);
        return -ENOMEM;
    }

    // Настройка диска
    strcpy(vdisk->disk_name, "vdisk");
    vdisk->major = 0; // Автоназначение
    vdisk->first_minor = 0;
    vdisk->fops = NULL;
    vdisk->queue = queue;
    set_capacity(vdisk, DISK_SIZE_SECTORS);

    // Регистрация диска
    add_disk(vdisk);
    pr_info("Virtual block device initialized\n");
    return 0;
}

static void __exit vdisk_exit(void) {
    del_gendisk(vdisk);
    blk_cleanup_queue(queue);
    put_disk(vdisk);
    vfree(storage);
    pr_info("Virtual block device removed\n");
}

modile_init(vdisk_init);
module_exit(vdisk_exit);
MODULE_LICENSE("GPL");