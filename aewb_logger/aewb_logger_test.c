#include "aewb_logger_sender.h"
#include "aewb_logger_receiver.h"

#ifdef NDEBUG
#error "make sure cmake CMAKE_BUILD_TYPE is set to Debug, otherwise assert will be nop"
#endif

void test_create_failure() {
    aewb_logger_sender_state_t *sender = aewb_logger_create_sender("333.444.555.666", 4321); // pass illegal ip
    aewb_logger_sender_state_t *receiver = aewb_logger_create_receiver("333.444.555.666", 4321); // pass illegal ip
    printf("%x", sender);
    printf("%x", receiver);
    assert(sender==NULL);
    assert(receiver==NULL);
}

void test_send_receive_bytes() {
    aewb_logger_sender_state_t *sender = aewb_logger_create_sender("192.168.5.3", 4321);
    aewb_logger_receiver_state_t *receiver = aewb_logger_create_receiver("192.168.5.3", 4321);

    char *sbuf = (char*)&sender->buffer;
    sbuf[0] = 'a';
    sbuf[sizeof(sender->buffer)-1] = 'z';

    int32_t n_sent = aewb_logger_send_bytes(sender);
    assert(n_sent==sizeof(sender->buffer));

    int32_t n_recvd = aewb_logger_recv_bytes(receiver);
    assert(n_recvd==sizeof(sender->buffer));

    char *rbuf = (char*)&receiver->buffer;
    assert(rbuf[0]=='a');
    assert(rbuf[sizeof(receiver->buffer)-1]=='z');

    aewb_logger_destroy_sender(sender);
    aewb_logger_destroy_receiver(receiver);
}

void test_write_read_log()
{
    AewbHandle sender_handle;
    dcc_parser_input_params_t sender_dcc_input_params;
    awbprm_t sender_awb_params;
    tiae_prm_t sender_tiae_prm;
    tivx_h3a_data_t sender_h3a;
    tivx_ae_awb_params_t sender_ae_awb_result;
    

    sender_handle.ti_2a_wrapper.dcc_input_params = &sender_dcc_input_params;
    sender_handle.ti_2a_wrapper.p_ae_params = &sender_tiae_prm;  
    sender_handle.ti_2a_wrapper.p_awb_params = &sender_awb_params;

    sender_handle.aewb_config.awb_num_skip_frames = 121;
    sender_handle.ti_2a_wrapper.frame_count = 1313;
    sender_handle.ti_2a_wrapper.dcc_input_params->exposure_time = 1414;
    sender_handle.ti_2a_wrapper.p_ae_params->exposure_program.exposure_time_range[0].min = 1515;
    sender_handle.sensor_in_data.ae_dynPrms.targetBrightness = 1616;
    sender_handle.sensor_out_data.aePrms.exposureTime[0] = 1717;
    sender_h3a.aew_config.aewwin1_WINW = 1818;
    sender_ae_awb_result.wb_gains[3] = 1919;
    
    log_aewb_message_t buffer;

    int32_t n_written;
    n_written = aewb_logger_write_log_to_buffer(&sender_handle, &sender_h3a, &sender_ae_awb_result, &buffer);

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
    AewbHandle sender_handle;
    dcc_parser_input_params_t sender_dcc_input_params;
    awbprm_t sender_awb_params;
    tiae_prm_t sender_tiae_prm;
    tivx_h3a_data_t sender_h3a;
    tivx_ae_awb_params_t sender_ae_awb_result;

    sender_handle.ti_2a_wrapper.dcc_input_params = &sender_dcc_input_params;
    sender_handle.ti_2a_wrapper.p_ae_params = &sender_tiae_prm;  
    sender_handle.ti_2a_wrapper.p_awb_params = &sender_awb_params;

    sender_handle.aewb_config.awb_num_skip_frames = 121;

    aewb_logger_sender_state_t *sender = aewb_logger_create_sender("192.168.5.3", 4321);
    aewb_logger_receiver_state_t *receiver = aewb_logger_create_receiver("192.168.5.3", 4321);

    int32_t n_written = aewb_logger_send_log(sender, &sender_handle, &sender_h3a, &sender_ae_awb_result);
    int32_t n_read = aewb_logger_recv_log(receiver);

    assert(n_written==sizeof(log_aewb_message_t));
    assert(n_written==n_read);
    assert(receiver->buffer.handle.aewb_config.awb_num_skip_frames == 121);

    aewb_logger_destroy_sender(sender);
    aewb_logger_destroy_receiver(receiver);    
}

int main() 
{
    test_send_receive_bytes();
    test_write_read_log();
    test_send_recv_log();
    test_create_failure();
}
