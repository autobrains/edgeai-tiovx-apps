#include "aewb_logger_sender.h"
#include "aewb_logger_receiver.h"

#ifdef NDEBUG
#error "make sure cmake CMAKE_BUILD_TYPE is set to Debug, otherwise assert will be nop"
#endif

void test_create_failure() {
    aewb_logger_sender_state_t *sender = aewb_logger_create_sender("333.444.555.666", 4321); // pass illegal ip
    aewb_logger_receiver_state_t *receiver = aewb_logger_create_receiver("333.444.555.666", 4321); // pass illegal ip
    assert (sender == NULL);
    assert (receiver == NULL);
}

void test_send_receive_bytes() {
    aewb_logger_sender_state_t *sender = aewb_logger_create_sender("192.168.5.3", 4321);
    aewb_logger_receiver_state_t *receiver = aewb_logger_create_receiver("192.168.5.3", 4321);

    char *sbuf = (char *)&sender->buffer;
    sbuf[0] = 'a';
    sbuf[sizeof(sender->buffer) - 1] = 'z';

    int32_t n_sent = aewb_logger_send_bytes(sender);
    assert(n_sent==sizeof(sender->buffer));

    int32_t n_recvd = aewb_logger_recv_bytes(receiver);
    assert(n_recvd==sizeof(sender->buffer));

    char *rbuf = (char *)&receiver->buffer;
    assert(rbuf[0]=='a');
    assert(rbuf[sizeof(receiver->buffer) - 1] == 'z');

    aewb_logger_destroy_sender(sender);
    aewb_logger_destroy_receiver(receiver);
}

void build_sender_args(aewb_logger_sender_args_t *args) {
    args->ti_2a_wrapper = (TI_2A_wrapper*)malloc(sizeof(TI_2A_wrapper));
    args->sensor_in_data = (sensor_config_get*)malloc(sizeof(sensor_config_get)); 
    args->sensor_out_data = (sensor_config_set*)malloc(sizeof(sensor_config_set));
    args->aewb_config = (tivx_aewb_config_t*)malloc(sizeof(tivx_aewb_config_t));
    args->h3a_data = (tivx_h3a_data_t *)malloc(sizeof(tivx_h3a_data_t));
    args->ae_awb_result = (tivx_ae_awb_params_t *)malloc(sizeof(tivx_ae_awb_params_t));

    args->ti_2a_wrapper->dcc_input_params = (dcc_parser_input_params_t*)malloc(sizeof(dcc_parser_input_params_t));
    args->ti_2a_wrapper->p_ae_params = (tiae_prm_t*)malloc(sizeof(tiae_prm_t));
    args->ti_2a_wrapper->p_awb_params = (awbprm_t*)malloc(sizeof(awbprm_t));
}

void test_write_read_log()
{
    aewb_logger_sender_args_t sender_args;
    build_sender_args(&sender_args);

    sender_args.aewb_config->awb_num_skip_frames = 121;
    sender_args.ti_2a_wrapper->frame_count = 1313;
    sender_args.ti_2a_wrapper->dcc_input_params->exposure_time = 1414;
    sender_args.ti_2a_wrapper->p_ae_params->exposure_program.exposure_time_range[0].min = 1515;
    sender_args.sensor_in_data->ae_dynPrms.targetBrightness = 1616;
    sender_args.sensor_out_data->aePrms.exposureTime[0] = 1717;
    sender_args.h3a_data->aew_config.aewwin1_WINW = 1818;
    sender_args.ae_awb_result->wb_gains[3] = 1919;
    
    log_aewb_message_t buffer;

    int32_t n_written;
    n_written = aewb_logger_write_log_to_buffer(&sender_args, &buffer);

    printf("test_write_read_log: n_written=%d\n",n_written);
    assert(n_written==sizeof(log_AewbHandle));

    assert(buffer.handle.aewb_config.awb_num_skip_frames == 121);
    assert(buffer.handle.ti_2a_wrapper.frame_count == 1313);
    assert(buffer.handle.ti_2a_wrapper.dcc_input_params.exposure_time == 1414);
    assert(buffer.handle.ti_2a_wrapper.ae_params.exposure_program.exposure_time_range[0].min == 1515);
    assert(buffer.handle.ae_dynPrms.targetBrightness == 1616);
    assert(buffer.handle.aePrms.exposureTime[0] == 1717);
    assert(buffer.h3a_data.aew_config.aewwin1_WINW == 1818);
    assert(buffer.ae_awb_result.wb_gains[3]==1919);
}

void test_send_recv_log()
{
    aewb_logger_sender_args_t sender_args;
    build_sender_args(&sender_args);

    sender_args.aewb_config->awb_num_skip_frames = 121;

    aewb_logger_sender_state_t *sender = aewb_logger_create_sender("192.168.5.3", 4321);
    aewb_logger_receiver_state_t *receiver = aewb_logger_create_receiver("192.168.5.3", 4321);
    frame_brightness_params_t avg_brightness;

    int32_t n_written = aewb_logger_send_log(
        sender,
        sender_args.ti_2a_wrapper,
        sender_args.sensor_in_data,
        sender_args.sensor_out_data,
        sender_args.aewb_config,
        sender_args.h3a_data,
        sender_args.ae_awb_result,
        &avg_brightness
    );

    int32_t n_read = aewb_logger_recv_log(receiver);

    assert(n_written==sizeof(log_aewb_message_t));
    assert(n_written==n_read);
    assert(receiver->buffer.handle.aewb_config.awb_num_skip_frames == 121);

    aewb_logger_destroy_sender(sender);
    aewb_logger_destroy_receiver(receiver);    
}

int main() 
{
    printf("\ntest_send_receive_bytes\n");
    test_send_receive_bytes();

    printf("\ntest_write_read_log\n");
    test_write_read_log();

    printf("\ntest_send_recv_log\n");
    test_send_recv_log();

    printf("\ntest_create_failure\n");
    test_create_failure();
}
