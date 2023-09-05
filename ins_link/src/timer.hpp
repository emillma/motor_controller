bool repeating_timer_callback(struct repeating_timer *t)
{
    uint32_t time = to_ms_since_boot(get_absolute_time());
    return true;
}

void init_timer()
{
    struct repeating_timer timer;
    add_repeating_timer_ms(-500, repeating_timer_callback, NULL, &timer);
}