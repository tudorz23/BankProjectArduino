#include "utils.h"
#include "debounce.h"
#include "wdt_counter.h"

/* FUNCTION DEFINITIONS */

void extract_uid(char *buff) {
    // Read one byte (i.e. 2 hex chars) at a time.
    // Pad with one 0 to the left if the byte can be
    // written with only one hex char (i.e. curr_byte < 0x10).
    for (uint8_t i = 0; i < mfrc522.uid.size; i++) {
        sprintf(&buff[i * 2], "%02X", mfrc522.uid.uidByte[i]);
    }

    // Add null-terminator.
    buff[mfrc522.uid.size * 2] = '\0';
}


int8_t get_user_idx_from_uid(char *uid) {
    for (uint8_t i = 0; i < MAX_USERS; i++) {
        if (strcmp(users[i].uid, uid) == 0) {
            return i;
        }
    }

    return NO_USER;
}


uint32_t read_number_input(ReadInputType type) {
    // If a PIN is read, limit it to 4 digits, if a sum is read, limit it to MAX_SUM_DIGITS.
    uint8_t max_digits = type == ReadInputType::PIN ? PIN_SIZE : MAX_SUM_DIGITS;

    uint32_t number = 0;
    uint8_t read_key = 0;
    uint8_t idx = 0;
    uint8_t last_key = 0;  // Keep track of last key pressed

    while (true) {
        // If red button is pressed ,return 0 to signal abort.
        if (is_red_button_pressed()) {
            return 0;
        }

        // If joystick button is pressed.
        if (is_joy_button_pressed()) {
            if (type == ReadInputType::PIN && idx == PIN_SIZE) {
                return number;
            }

            if (type == ReadInputType::SUM && idx > 0) {
                return number;
            }
        }

        // Read current key (non-blocking).
        read_key = ttp229.GetKey16();

        // Only react if no key was previously pressed (to avoid accidental multiple sampling).
        if (read_key != 0 && last_key == 0) {
            // '0' key
            if (read_key == 10) {
                if (idx != 0 && idx < max_digits) {
                    number = number * 10;
                    idx++;
                }
            }

            // Backspace
            else if (read_key == 11) {
                // Can't delete if there is no digit.
                if (idx != 0) {
                    number = number / 10;
                    idx--;
                }
            }

            // Normal digits
            else if (read_key <= 9) { 
                if (idx < max_digits) {
                    number = number * 10 + read_key;
                    idx++;
                }
            }

            // Print intermediary number
            lcd.setCursor(0, 1);
            lcd.print(BLANK); // clear
            lcd.setCursor(0, 1);
            if (number != 0) {
                lcd.print(number);
            }
        }

        // Update last_key
        last_key = read_key;
    }
}


void register_user(uint8_t idx) {
    registered_users |= (1 << idx);
}


bool is_user_registered(uint8_t idx) {
    return (registered_users & (1 << idx)) != 0;
}


void apply_interest(User &user) {
    // The wdt_counter is incremented every 1 second.
    uint16_t elapsed_time = get_wdt_counter() - user.last_interest_update_time;

    #ifdef DEBUG
    Serial.println(user.last_interest_update_time);
    Serial.println(elapsed_time);
    #endif

    // The interest is applied every APPLY_INTEREST_INTERVAL seconds.
    uint8_t times_to_apply = elapsed_time / APPLY_INTEREST_INTERVAL;

    // Apply the interest times_to_apply times.
    for (uint8_t i = 0; i < times_to_apply; i++) {
        user.economy_sum += user.economy_sum * ((float)INTEREST_RATE / 100.0);
    }

    // Last update basically happened at last_time + times_to_apply * INTERVAL.
    user.last_interest_update_time += times_to_apply * APPLY_INTEREST_INTERVAL;

    #ifdef DEBUG
    Serial.println(user.last_interest_update_time);

    Serial.println();
    #endif
}


int8_t get_first_friend(uint8_t logged_user) {
    for (uint8_t idx = 0; idx < MAX_USERS; idx++) {
        if (idx == logged_user) {
            continue;
        }

        if (friendships[logged_user][idx]) {
            return idx;
        }
    }

    // No friend found.
    return -1;
}


uint8_t get_prev_friend(uint8_t logged_user, uint8_t curr_friend) {
    for (int8_t idx = curr_friend - 1; idx >= 0; idx--) {
        if (idx == logged_user) {
            continue;
        }

        if (friendships[logged_user][idx]) {
            return idx;
        }
    }

    return curr_friend;
}


uint8_t get_next_friend(uint8_t logged_user, uint8_t curr_friend) {
    for (uint8_t idx = curr_friend + 1; idx < MAX_USERS; idx++) {
        if (idx == logged_user) {
            continue;
        }

        if (friendships[logged_user][idx]) {
            return idx;
        }
    }

    return curr_friend;
}


int8_t get_first_friend_candidate(uint8_t logged_user) {
    for (uint8_t idx = 0; idx < MAX_USERS; idx++) {
        if (idx == logged_user) {
            continue;
        }
        
        // If not registered, or if already friends, or if a friend request is in progress.
        if (!is_user_registered(idx) || friendships[logged_user][idx]
            || sent_friend_req[logged_user][idx] || sent_friend_req[idx][logged_user]) {
            continue;
        }

        return idx;
    }

    return -1;
}


uint8_t get_prev_friend_candidate(uint8_t logged_user, uint8_t curr_candidate) {
    for (int8_t idx = curr_candidate - 1; idx >= 0; idx--) {
        if (idx == logged_user) {
            continue;
        }

        // If not registered, or if already friends, or if a friend request is in progress.
        if (!is_user_registered(idx) || friendships[logged_user][idx]
            || sent_friend_req[logged_user][idx] || sent_friend_req[idx][logged_user]) {
            continue;
        }

        return idx;
    }

    return curr_candidate;
}


uint8_t get_next_friend_candidate(uint8_t logged_user, uint8_t curr_candidate) {
    for (uint8_t idx = curr_candidate + 1; idx < MAX_USERS; idx++) {
        if (idx == logged_user) {
            continue;
        }

        // If not registered, or if already friends, or if a friend request is in progress.
        if (!is_user_registered(idx) || friendships[logged_user][idx]
            || sent_friend_req[logged_user][idx] || sent_friend_req[idx][logged_user]) {
            continue;
        }

        return idx;
    }

    return curr_candidate;
}


uint8_t get_prev_notification(uint8_t curr_notif) {
    if (curr_notif == 0) {
        return 0;
    }

    return curr_notif - 1;
}


uint8_t get_next_notification(uint8_t notif_cnt, uint8_t curr_notif) {
    if (curr_notif + 1 == notif_cnt) {
        return curr_notif;
    }

    return curr_notif + 1;
}


void add_notification_to_inbox(uint8_t to_who, uint8_t from_who, NotifType type, uint32_t sum) {
    // If the inbox is full, don't add the notification.
    uint8_t curr_count = users[to_who].notif_cnt;

    if (curr_count == MAX_NOTIFS) {
        return;
    }

    users[to_who].notifications[curr_count].type = type;
    users[to_who].notifications[curr_count].from_who = from_who;
    users[to_who].notifications[curr_count].sum = sum;

    // Increment user's notification counter.
    users[to_who].notif_cnt++;
}


uint8_t mark_notif_as_seen(uint8_t logged_user, uint8_t notif_idx) {
    uint8_t curr_count = users[logged_user].notif_cnt;

    if (curr_count == 0) {
        return 0;
    }

    // Remove the notification from the inbox and slide the ones that come
    // after it one position to the left.
    for (uint8_t next_idx = notif_idx + 1; next_idx < curr_count; next_idx++) {
        Notification &curr = users[logged_user].notifications[notif_idx];
        Notification &next = users[logged_user].notifications[next_idx];

        curr.type = next.type;
        curr.from_who = next.from_who;
        curr.sum = next.sum;

        notif_idx++;
    }

    // Decrement user's notification counter.
    users[logged_user].notif_cnt--;
    return curr_count - 1;
}


void display_notification(Notification &notif) {
    lcd.clear();
    lcd.setCursor(0, 0);

    if (notif.type == NotifType::RecvFromFriend) {
        lcd.print(users[notif.from_who].name);
        lcd.setCursor(0, 1);
        lcd.print(F("sent "));
        lcd.print(notif.sum);
    }

    else if (notif.type == NotifType::FriendReq) {
        lcd.print(F("Friend req from"));
        lcd.setCursor(0, 1);
        lcd.print(users[notif.from_who].name);
    }

    else if (notif.type == NotifType::ReqAccepted) {
        lcd.print(users[notif.from_who].name);
        lcd.setCursor(0, 1);
        lcd.print(F("is now a friend"));
    }
}


void greet_logged_user() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Hello"));

    lcd.setCursor(0, 1);
    lcd.print(users[logged_user].name);
}
