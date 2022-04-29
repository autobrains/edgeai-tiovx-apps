TIOVX_MODULE_ERROR("EDGEAI_DATA_PATH Not Defined!!\n");


aewb_logger_state_t *aewb_logger_create_sender(const char *dest_ip, in_port_t port);
aewb_logger_state_t *aewb_logger_create_receiver(const char *bind_ip, in_port_t port);
void aewb_logger_destroy(aewb_logger_state_t *p_state);

int32_t aewb_logger_write_log_to_buffer(AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, tivx_ae_awb_params_t *aewb_ptr, int8_t * dest_buffer, int32_t buffer_size);
int32_t aewb_logger_read_log_from_buffer(int8_t * src_buffer, int32_t buffer_size, AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, tivx_ae_awb_params_t *aewb_ptr);

int32_t aewb_logger_send_log(aewb_logger_state_t *p_state, AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, tivx_ae_awb_params_t *aewb_ptr);
int32_t aewb_logger_recv_log(aewb_logger_state_t *p_state, AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, tivx_ae_awb_params_t *aewb_ptr);

