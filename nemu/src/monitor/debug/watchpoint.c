#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
static WP *cur;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].prior = &wp_pool[i - 1];
  }
  wp_pool[NR_WP - 1].next = wp_pool;
  wp_pool[0].prior = &wp_pool[NR_WP - 1];
  head = NULL;
  free_ = wp_pool;
  cur = head;
}

/* TODO: Implement the functionality of watchpoint */


WP *new_wp() {
  if (free_->next->NO == 0) {
    assert(0);
  } else {
    WP *res = free_->next;

    free_->next = res->next;
    free_->next->prior = free_;
    
    head->next->prior = res;
    res->prior = head;
    res->next = head->next;
    head->next = res;
    
    res->counter = 0;
    return res;
  }
}


int free_wp(int n) {
  if (n < 0 || n >= NR_WP) {
    return -1;
  }
  WP *wp = &wp_pool[n];
  
  wp->prior->next = wp->next;
  wp->next->prior = wp->prior;
  
  free_->next->prior = wp;
  wp->prior = free_;
  wp->next = free_->next;
  free_->next = wp;
  return 0;
}


int check_wp() {
  //WP *wp = head;
  while (cur->next->NO != 0) {
    cur = cur->next;
    if (cur->counter == 0) {
      continue;
    }
    cur->counter++;
    bool success;
    int val = expr(cur->expr, &success);
    if (success == false) {
      assert(0);
    } else if (val != cur->last_val) {
      printf("watchpoint #%d\t%s:\t%d\t->\t%d\n", cur->NO,
      cur->expr, cur->last_val, val);
      cur->last_val = val;
      return cur->NO;
    } else {
      cur->last_val = val;
    }
  }
  cur = head;
  return 0;
}

void print_wp() {
  while (cur->next->NO != 0) {
    cur = cur->next;
    printf("watchpoint #%d\t%s:\t%d\n", cur->NO,
      cur->expr, cur->last_val);
  }
  cur = head;
}