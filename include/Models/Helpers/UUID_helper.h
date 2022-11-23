// #pragma once

// #include <string>

// const char uuid_list[26][37] = {
//     "ad2c6d62-6414-4bca-9310-027afd7f408a",
//     "f83fe164-46b2-4ed6-9b92-18ab77fa8c54",
//     "6b869d9e-2384-4bfa-8be6-22b9bdbcf410",
//     "23ea3be9-917c-4c30-a2a5-52094123ff81",
//     "c4bf5152-b151-4fcf-b496-5069ea7fc793",
//     "09ab1652-b5a7-4742-a3fe-78e74cfeed7d",
//     "3416a0b9-5507-433a-82b8-acdb3208af57",
//     "b72c2c7a-6b9b-4eee-82e7-b04983efca6d",
//     "41d4fe9f-0f2e-447f-82dc-a71e9c684f38",
//     "bd9286f9-f68c-419b-90f3-69d570fea9e6",
//     "0592b5b5-65be-4382-bcd2-0f8a54465ef8",
//     "73d79bc7-847a-4aaa-8420-ba8cf50da7ab",
//     "2cc0f92e-12d0-49d2-9d0e-2fa4ea27c525",
//     "25ebbc6f-6166-4e17-a3f8-cce9e3735a97",
//     "bfe699ec-0935-4ebb-859a-3b0512d14780",
//     "cb4e9606-267f-4494-8974-2b10c64ea6da",
//     "584cd8cd-ac8d-417a-84bb-941e846e54f8",
//     "af18cf34-4589-481a-bc23-28c663652df7",
//     "c965b737-95fb-461e-b64a-918b188e6b6d",
//     "94d42814-dd81-4ddf-ae52-cd9ac7c218a9",
//     "4f509bf7-1e77-4f3a-8852-3d86fc48b344",
//     "4efc5750-2abb-425c-801d-d6d111f83cb7",
//     "e525d474-a055-4f39-9387-6c46e54fcd78",
//     "25a62c3f-e74e-4dfd-96c2-bd9da4cac697",
//     "d268e4e3-c6f9-491b-a149-fee17cbc3ed9"
// };
// const int uuid_qty = sizeof uuid_list / sizeof uuid_list[0];

// namespace Helper
// {
//     class uuid_helper
//     {
//         public:
//             // Gives a unique UUID from limited UUID List. The same UUID won't be given twice. Empty string if no more UUID is available.
//             static std::string AssignUUID()
//             {
//                 if(__counter <= uuid_qty)
//                 {
//                     return uuid_list[__counter++];
//                 }
//                 return "";
//             }

//         private:
//             uuid_helper() { };
//             static int __counter;
//     };
//     int uuid_helper::__counter = 0;
// }
